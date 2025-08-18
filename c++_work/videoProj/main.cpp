#define XDMA_TEST 1

#if XDMA_TEST == 0
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <rockchip/rk_mpi.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>

#define XDMA_DEV "/dev/xdma0"
#define BUF_SIZE (1024*1024)

int main() {
    // 1. 初始化XDMA
    int xdma_fd = open(XDMA_DEV, O_RDWR);
    void *pBuf;
    posix_memalign(&pBuf, 4096, BUF_SIZE);
    void *xdma_buf = mmap(pBuf, BUF_SIZE, PROT_READ|PROT_WRITE,
                          MAP_SHARED|MAP_FIXED, xdma_fd, 0);

    // 2. 初始化MPP
    RK_U32 ret = 0;
    MppCtx ctx;
    MppApi *mpi;
    MppPacket packet;
    MppFrame frame;
    mpp_create(&ctx, &mpi);
    mpp_init(ctx, MPP_CTX_DEC, MPP_VIDEO_CodingAVC);

    while(1) {
        // 3. 从XDMA读取数据
        ssize_t len = read(xdma_fd, xdma_buf, BUF_SIZE);

        // 4. 解码处理
        mpp_packet_init(&packet, xdma_buf, len);
        mpi->decode_put_packet(ctx, packet);

        // ...帧处理逻辑
        // 获取解码帧
        do {
            ret = mpi->decode_get_frame(ctx, &frame);
            if (ret || !frame) break;

            // 处理解码结果（YUV数据）
            if (0) {
                // 错误处理
            } else {
                MppBuffer buf = mpp_frame_get_buffer(frame);
                void *yuv_data = mpp_buffer_get_ptr(buf);
                int width = mpp_frame_get_width(frame);
                int height = mpp_frame_get_height(frame);
                // TODO: 输出到RGA/DRM显示
            }
            mpp_frame_deinit(&frame);
        } while (1);
        mpp_packet_deinit(&packet);
    }

    // 资源释放
    munmap(xdma_buf, BUF_SIZE);
    free(pBuf);
    close(xdma_fd);
    mpp_destroy(ctx);
    return 0;
}
#elif XDMA_TEST == 1
#include <iostream>
#include <thread>
#include <chrono>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <libdrm/drm_fourcc.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

// DRM显示结构体
struct DrmDisplay {
    int fd;
    uint32_t conn_id;
    uint32_t crtc_id;
    drmModeModeInfo mode;
    uint32_t plane_id;
    uint32_t fb_id;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint8_t* map;
};

// 初始化DRM显示
bool init_drm(DrmDisplay* disp, const char* device = "/dev/dri/card0") {
    disp->fd = open(device, O_RDWR | O_CLOEXEC);
    if (disp->fd < 0) {
        perror("Failed to open DRM device");
        return false;
    }

    drmModeRes* res = drmModeGetResources(disp->fd);
    if (!res) {
        perror("drmModeGetResources failed");
        return false;
    }

    // 查找第一个连接的显示器
    bool connector_found = false;
    for (int i = 0; i < res->count_connectors; i++) {
        drmModeConnector* conn = drmModeGetConnector(disp->fd, res->connectors[i]);
        if (conn && conn->connection == DRM_MODE_CONNECTED) {
            disp->conn_id = conn->connector_id;
            disp->mode = conn->modes[0];  // 使用第一个可用模式
            connector_found = true;
            drmModeFreeConnector(conn);
            break;
        }
        if (conn) drmModeFreeConnector(conn);
    }

    if (!connector_found) {
        fprintf(stderr, "No connected display found\n");
        drmModeFreeResources(res);
        return false;
    }

    // 查找CRTC
    disp->crtc_id = res->crtcs[0];  // 通常使用第一个CRTC

    // 查找Overlay平面
    drmModePlaneRes* planes = drmModeGetPlaneResources(disp->fd);
    if (!planes) {
        perror("drmModeGetPlaneResources failed");
        drmModeFreeResources(res);
        return false;
    }

    bool plane_found = false;
    for (uint32_t i = 0; i < planes->count_planes; i++) {
        drmModePlane* plane = drmModeGetPlane(disp->fd, planes->planes[i]);
        if (plane) {
            // 检查平面类型（OVERLAY或PRIMARY）
            if (plane->possible_crtcs & (1 << 0)) {  // 假设CRTC索引为0
                disp->plane_id = plane->plane_id;
                plane_found = true;
                drmModeFreePlane(plane);
                break;
            }
            drmModeFreePlane(plane);
        }
    }

    drmModeFreePlaneResources(planes);
    drmModeFreeResources(res);

    if (!plane_found) {
        fprintf(stderr, "No suitable plane found\n");
        return false;
    }

    return true;
}

// 创建DRM FrameBuffer
bool create_drm_fb(DrmDisplay* disp, uint32_t width, uint32_t height, uint32_t format) {
    disp->width = width;
    disp->height = height;

    // 创建FrameBuffer对象
    struct drm_mode_create_dumb create_dumb = {0};
    create_dumb.width = width;
    create_dumb.height = height;
    create_dumb.bpp = 32; // 32位像素格式

    if (drmIoctl(disp->fd, DRM_IOCTL_MODE_CREATE_DUMB, &create_dumb) < 0) {
        perror("DRM_IOCTL_MODE_CREATE_DUMB failed");
        return false;
    }

    disp->pitch = create_dumb.pitch;

    // 映射内存
    struct drm_mode_map_dumb map_dumb = {0};
    map_dumb.handle = create_dumb.handle;

    if (drmIoctl(disp->fd, DRM_IOCTL_MODE_MAP_DUMB, &map_dumb) < 0) {
        perror("DRM_IOCTL_MODE_MAP_DUMB failed");
        return false;
    }

    disp->map = static_cast<uint8_t*>(mmap(0, create_dumb.size,
                                           PROT_READ | PROT_WRITE,
                                           MAP_SHARED,
                                           disp->fd,
                                           map_dumb.offset));
    if (disp->map == MAP_FAILED) {
        perror("mmap failed");
        return false;
    }

    // 添加FrameBuffer
    uint32_t handles[4] = {create_dumb.handle, 0, 0, 0};
    uint32_t pitches[4] = {disp->pitch, 0, 0, 0};
    uint32_t offsets[4] = {0, 0, 0, 0};

    if (drmModeAddFB2(disp->fd, width, height, format,
                      handles, pitches, offsets, &disp->fb_id, 0)) {
        perror("drmModeAddFB2 failed");
        return false;
    }

    return true;
}

// 显示帧到屏幕
void display_frame(DrmDisplay* disp, uint8_t* data, int width, int height) {
    // 计算居中位置
    int x = (disp->mode.hdisplay - width) / 2;
    int y = (disp->mode.vdisplay - height) / 2;

    // 复制数据到DRM缓冲区
    for (int i = 0; i < height; i++) {
        memcpy(disp->map + i * disp->pitch,
               data + i * width * 4,
               width * 4);
    }

    // 设置平面显示
    drmModeSetPlane(disp->fd, disp->plane_id, disp->crtc_id, disp->fb_id, 0,
                    x, y, width, height,
                    0, 0, width << 16, height << 16);
}

// FFmpeg解码和播放函数
void play_mp4(const char* filename, DrmDisplay* disp) {
    AVFormatContext* fmt_ctx = nullptr;
    AVCodecContext* codec_ctx = nullptr;
    AVCodec* codec = nullptr;
    AVFrame* frame = nullptr;
    AVFrame* rgb_frame = nullptr;
    AVPacket pkt;
    SwsContext* sws_ctx = nullptr;

    // 打开视频文件
    if (avformat_open_input(&fmt_ctx, filename, nullptr, nullptr) != 0) {
        std::cerr << "Could not open video file" << std::endl;
        return;
    }

    // 查找流信息
    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
        std::cerr << "Could not find stream information" << std::endl;
        avformat_close_input(&fmt_ctx);
        return;
    }

    // 查找视频流
    int video_stream = -1;
    for (unsigned int i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream = i;
            break;
        }
    }

    if (video_stream == -1) {
        std::cerr << "No video stream found" << std::endl;
        avformat_close_input(&fmt_ctx);
        return;
    }

    // 获取解码器
    AVCodecParameters* codec_params = fmt_ctx->streams[video_stream]->codecpar;
    codec = avcodec_find_decoder(codec_params->codec_id);
    if (!codec) {
        std::cerr << "Unsupported codec" << std::endl;
        avformat_close_input(&fmt_ctx);
        return;
    }

    // 创建解码器上下文
    codec_ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_ctx, codec_params);

    // 打开解码器
    if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
        std::cerr << "Could not open codec" << std::endl;
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return;
    }

    // 分配帧
    frame = av_frame_alloc();
    rgb_frame = av_frame_alloc();
    if (!frame || !rgb_frame) {
        std::cerr << "Could not allocate frames" << std::endl;
        goto cleanup;
    }

    // 准备RGB转换
    rgb_frame->format = AV_PIX_FMT_RGBA;
    rgb_frame->width = codec_ctx->width;
    rgb_frame->height = codec_ctx->height;
    av_frame_get_buffer(rgb_frame, 0);

    // 创建DRM FrameBuffer
    if (!create_drm_fb(disp, codec_ctx->width, codec_ctx->height, DRM_FORMAT_XRGB8888)) {
        std::cerr << "Failed to create DRM framebuffer" << std::endl;
        goto cleanup;
    }

    // 主解码循环
    while (av_read_frame(fmt_ctx, &pkt) >= 0) {
        if (pkt.stream_index == video_stream) {
            // 发送包到解码器
            int ret = avcodec_send_packet(codec_ctx, &pkt);
            if (ret < 0) {
                std::cerr << "Error sending packet to decoder" << std::endl;
                break;
            }

            // 接收解码后的帧
            while (ret >= 0) {
                ret = avcodec_receive_frame(codec_ctx, frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                } else if (ret < 0) {
                    std::cerr << "Error during decoding" << std::endl;
                    break;
                }

                // 创建缩放上下文
                sws_ctx = sws_getCachedContext(sws_ctx,
                                               frame->width, frame->height,
                                               static_cast<AVPixelFormat>(frame->format),
                                               codec_ctx->width, codec_ctx->height,
                                               AV_PIX_FMT_RGBA,
                                               SWS_BILINEAR, nullptr, nullptr, nullptr);

                if (!sws_ctx) {
                    std::cerr << "Could not create scale context" << std::endl;
                    break;
                }

                // 转换到RGB
                sws_scale(sws_ctx,
                          frame->data, frame->linesize,
                          0, frame->height,
                          rgb_frame->data, rgb_frame->linesize);

                // 显示帧
                display_frame(disp, rgb_frame->data[0],
                              codec_ctx->width, codec_ctx->height);

                // 控制帧率
                double fps = av_q2d(fmt_ctx->streams[video_stream]->avg_frame_rate);
                if (fps > 0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(
                            static_cast<int>(1000 / fps)));
                }
            }
        }
        av_packet_unref(&pkt);
    }

    // 刷新解码器
    pkt.data = nullptr;
    pkt.size = 0;
    avcodec_send_packet(codec_ctx, &pkt);

    cleanup:
    // 清理资源
    if (sws_ctx) sws_freeContext(sws_ctx);
    if (frame) av_frame_free(&frame);
    if (rgb_frame) av_frame_free(&rgb_frame);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&fmt_ctx);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <video.mp4>" << std::endl;
        return 1;
    }

    // 初始化DRM
    DrmDisplay disp;
    if (!init_drm(&disp)) {
        std::cerr << "DRM initialization failed" << std::endl;
        return 1;
    }

    // 注册FFmpeg组件
    av_register_all();
    avformat_network_init();

    // 播放视频
    play_mp4(argv[1], &disp);

    // 清理DRM资源
    if (disp.fd >= 0) {
        if (disp.fb_id) drmModeRmFB(disp.fd, disp.fb_id);
        close(disp.fd);
    }

    return 0;
}
#elif XDMA_TEST == 2
#include <fstream>
#include <iostream>
#include <rockchip/rk_mpi.h>

int main() {
    // 1. 打开H264文件
    std::ifstream file("test.h264", std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file" << std::endl;
        return -1;
    }

    // 2. 初始化MPP解码器
    MppCtx ctx = nullptr;
    MppApi* mpi = nullptr;
    MppBufferGroup buf_grp = nullptr;
    RK_U32 ret = 0;

    // 创建MPP上下文
    mpp_create(&ctx, &mpi);
    mpp_init(ctx, MPP_CTX_DEC, MPP_VIDEO_CodingAVC);  // AVC即H264
    MppParam param = {0};
    RK_U32 split_mode = 1;
    param = &split_mode;
    // 配置解码器参数
    ret = mpi->control(ctx, MPP_DEC_SET_PARSER_SPLIT_MODE, param);

    // 3. 分块读取并解码
    const size_t CHUNK_SIZE = 8192;  // 8KB数据块
    char buffer[CHUNK_SIZE];
    MppPacket packet = nullptr;
    MppFrame frame = nullptr;

    while (!file.eof()) {
        file.read(buffer, CHUNK_SIZE);
        size_t read_size = file.gcount();

        if (read_size > 0) {
            // 创建MPP数据包
            mpp_packet_init(&packet, buffer, read_size);

            // 送入解码器
            RK_S32 ret = mpi->decode_put_packet(ctx, packet);
            if (ret != MPP_OK) {
                std::cerr << "Failed to put packet" << std::endl;
            }
            mpp_packet_deinit(&packet);

            // 获取解码后的帧
            while (MPP_OK == mpi->decode_get_frame(ctx, &frame)) {
                if (frame) {
                    // 4. 处理解码帧 (YUV420格式)
                    MppBuffer buf = mpp_frame_get_buffer(frame);
                    if (buf) {
                        void* yuv_data = mpp_buffer_get_ptr(buf);
                        int width = mpp_frame_get_width(frame);
                        int height = mpp_frame_get_height(frame);

                        // 这里添加帧处理代码 (渲染/分析/转码等)
                        std::cout << "Decoded frame: "
                                  << width << "x" << height
                                  << " stride: " << mpp_frame_get_hor_stride(frame)
                                  << std::endl;
                    }
                    mpp_frame_deinit(&frame);
                }
            }
        }
    }

    // 5. 清理资源
    mpp_destroy(ctx);
    file.close();
    return 0;
}

#else
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <rockchip/rk_mpi.h>


#define H264_FILE_PATH "test.h264"
#define BUFFER_SIZE   1920 * 1080 * 3 / 2

int main() {
    MppCtx mpp_ctx = NULL;
    MppApi *mpp_api = NULL;
    MppPacket packet = NULL;
    MppFrame frame = NULL;
    MppBuffer readBuffer = NULL;
    MppBuffer writeBuffer = NULL;
    MppBufferGroup bufferGroup = NULL;
    FILE *h264_file = NULL;
    size_t file_size = 0;
    int ret = 0;

    // 创建 MPP 上下文并初始化
    ret = mpp_create(&mpp_ctx, &mpp_api);
    if (ret != MPP_OK) {
        printf("MPP create failed\n");
        return -1;
    }
    RK_U32 need_split = 1;
    ret= mpp_api->control(mpp_ctx, MPP_DEC_SET_PARSER_SPLIT_MODE, &need_split);
    if (ret != MPP_OK) {
        printf("MPP control split failed\n");
        mpp_destroy(mpp_ctx);
        return -1;
    }
    // 初始化 MPP（设置解码器为 H.264）
    ret = mpp_init(mpp_ctx, MPP_CTX_DEC, MPP_VIDEO_CodingAVC);
    if (ret != MPP_OK) {
        printf("MPP init failed\n");
        mpp_destroy(mpp_ctx);
        return -1;
    }

    // 打开 H.264 文件并获取大小
    h264_file = fopen(H264_FILE_PATH, "rb");
    if (!h264_file) {
        printf("Failed to open H.264 file\n");
        mpp_destroy(mpp_ctx);
        return -1;
    }
    fseek(h264_file, 0, SEEK_END);
    file_size = ftell(h264_file);
    fseek(h264_file, 0, SEEK_SET);
    mpp_buffer_group_get_internal(&bufferGroup, MPP_BUFFER_TYPE_ION);
    mpp_buffer_group_limit_config(bufferGroup, 0, 16);

    // 分配内存读取文件数据
    unsigned char *h264_data = (unsigned char *)malloc(file_size);
    if (!h264_data) {
        printf("Memory allocation failed\n");
        fclose(h264_file);
        mpp_destroy(mpp_ctx);
        return -1;
    }
    size_t cnt = fread(h264_data, 1, file_size, h264_file);
    if (cnt != file_size) {
        if (feof(h264_file))
            printf("已到文件末尾，仅读取 %zu 个整数\n", cnt);
        else if (ferror(h264_file))
            perror("读取错误");
        fclose(h264_file);
        free(h264_data);
        mpp_destroy(mpp_ctx);
        return -1;
    }
    fclose(h264_file);

    // 创建数据包
//    ret = mpp_packet_init(&packet, h264_data, file_size);
//    RK_U32 length = mpp_packet_get_length(packet);
//    printf("Received packet size: %d bytes\n", length);
//    if (ret != MPP_OK) {
//        printf("MPP packet init failed\n");
//        free(h264_data);
//        mpp_destroy(mpp_ctx);
//        return -1;
//    }

    // 解码视频流（异步接口）
    while (true) {
        mpp_buffer_get(bufferGroup, &readBuffer, BUFFER_SIZE);
        void* ptr = mpp_buffer_get_ptr(readBuffer);  // 获取内存指针
        memcpy(ptr, h264_data, BUFFER_SIZE);         // 拷贝数据
//        mpp_buffer_write(readBuffer, 0, h264_data, cnt);
        ret = mpp_packet_init(&packet, mpp_buffer_get_ptr(readBuffer), mpp_buffer_get_size(readBuffer));
        RK_U32 length = mpp_packet_get_length(packet);
        printf("Received packet size: %d bytes\n", length);
        if (ret != MPP_OK) {
            printf("MPP packet init failed\n");
            free(h264_data);
            mpp_destroy(mpp_ctx);
            return -1;
        }
        ret = mpp_api->decode_put_packet(mpp_ctx, packet);
        length = mpp_packet_get_length(packet);
        printf("Received packet size: %d bytes\n", length);
        if (ret == MPP_ERR_BUFFER_FULL) {
            // 缓冲区已满或等待更多数据，继续循环
            continue;
        } else if (ret != MPP_OK) {
            printf("Decode failed, error code: %d\n", ret);
            break;
        }

        // 获取解码后的帧
        ret = mpp_api->decode_get_frame(mpp_ctx, &frame);
        if (ret != MPP_OK || frame == NULL) {
            printf("Decode failed, error code: %d\n", ret);
            break;
        }

        // 获取解码帧的缓冲区
        mpp_buffer_get(bufferGroup, &writeBuffer, BUFFER_SIZE);
        writeBuffer = mpp_frame_get_buffer(frame);
        if (writeBuffer == NULL) {
            printf("Failed to get buffer from MppFrame\n");
            break;
        }

        // 获取 YUV 数据
        void *yuv_data = mpp_buffer_get_ptr(writeBuffer);
        size_t yuv_size = mpp_buffer_get_size(writeBuffer);
        printf("Decoded frame size: %zu bytes\n", yuv_size);

        // 这里可以添加代码将YUV数据通过DRM框架显示

        // 释放解码帧
        mpp_buffer_put(writeBuffer);
        mpp_buffer_put(readBuffer);
        mpp_frame_deinit(&frame);
    }

    // 释放资源
    free(h264_data);
    mpp_buffer_group_put(bufferGroup);
    mpp_api->reset(mpp_ctx);
    mpp_packet_deinit(&packet);
    mpp_destroy(mpp_ctx);

    return 0;
}

#endif

