////
//// Created by Administrator on 2025/8/12.
////
//
//#ifndef RK3588_PROJ_DISPLAY_H
//#define RK3588_PROJ_DISPLAY_H
//#include <fcntl.h>
//#include <unistd.h>
//#include <sys/mman.h>
//#include <xf86drm.h>
//#include <xf86drmMode.h>
//#include <cstring>
//#include <iostream>
//#include <stdexcept>
//#include <vector>
//#include <libdrm/drm_fourcc.h>
//
//class DrmDisplay {
//public:
//    // 显示模式配置
//    struct DisplayMode {
//        uint32_t width;
//        uint32_t height;
//        uint32_t refresh_rate;
//        drmModeModeInfo mode_info;
//    };
//
//    // 缓冲区结构
//    struct DisplayBuffer {
//        uint32_t handle;
//        uint32_t fb_id;
//        uint32_t pitch;
//        size_t size;
//        void* map;
//    };
//
//    // 构造函数
//    DrmDisplay(const char* device = "/dev/dri/card0",
//               uint32_t width = 1920,
//               uint32_t height = 1080)
//            : fd_(-1), crtc_id_(0), connector_id_(0),
//              width_(width), height_(height),
//              current_buffer_(0) {
//        open_device(device);
//        setup_display();
//        create_buffers(2); // 默认双缓冲
//    }
//
//    // 析构函数
//    ~DrmDisplay() {
//        cleanup();
//    }
//
//    // 获取当前显示模式
//    DisplayMode get_current_mode() const {
//        return current_mode_;
//    }
//
//    // 获取可用模式列表
//    const std::vector<DisplayMode>& get_available_modes() const {
//        return available_modes_;
//    }
//
//    // 设置显示模式
//    void set_mode(uint32_t width, uint32_t height, uint32_t refresh_rate = 0) {
//        for (const auto& mode : available_modes_) {
//            if (mode.width == width && mode.height == height &&
//                (refresh_rate == 0 || mode.refresh_rate == refresh_rate)) {
//                set_crtc(mode.mode_info);
//                width_ = width;
//                height_ = height;
//                recreate_buffers();
//                return;
//            }
//        }
//        throw std::runtime_error("Requested display mode not available");
//    }
//
//    // 获取帧缓冲区指针
//    void* get_framebuffer() {
//        return buffers_[current_buffer_].map;
//    }
//
//    // 获取下一帧缓冲区指针（用于双缓冲）
//    void* get_next_framebuffer() {
//        return buffers_[(current_buffer_ + 1) % buffers_.size()].map;
//    }
//
//    // 提交帧到显示
//    void flip_buffer() {
//        // 获取下一缓冲区的索引
//        uint32_t next_buffer = (current_buffer_ + 1) % buffers_.size();
//
//        // 设置页面翻转
//        drmModePageFlip(fd_, crtc_id_, buffers_[next_buffer].fb_id,
//                        DRM_MODE_PAGE_FLIP_EVENT, nullptr);
//
//        // 等待垂直同步
//        wait_for_vblank();
//
//        // 更新当前缓冲区索引
//        current_buffer_ = next_buffer;
//    }
//
//    // 等待垂直同步
//    void wait_for_vblank() {
//        drmVBlank vbl;
//        memset(&vbl, 0, sizeof(vbl));
//        vbl.request.type = DRM_VBLANK_RELATIVE;
//        vbl.request.sequence = 1;
//        drmWaitVBlank(fd_, &vbl);
//    }
//
//    // 获取文件描述符（用于事件处理）
//    int get_fd() const {
//        return fd_;
//    }
//
//private:
//    int fd_;                      // DRM文件描述符
//    uint32_t crtc_id_;            // CRTC ID
//    uint32_t connector_id_;       // 连接器ID
//    uint32_t width_;              // 当前宽度
//    uint32_t height_;             // 当前高度
//    DisplayMode current_mode_;    // 当前显示模式
//    std::vector<DisplayMode> available_modes_; // 可用显示模式
//    std::vector<DisplayBuffer> buffers_;       // 显示缓冲区
//    size_t current_buffer_;       // 当前显示缓冲区索引
//
//    // 打开DRM设备
//    void open_device(const char* device) {
//        fd_ = open(device, O_RDWR | O_CLOEXEC);
//        if (fd_ < 0) {
//            throw std::runtime_error("Failed to open DRM device");
//        }
//    }
//
//    // 设置显示
//    void setup_display() {
//        // 获取资源
//        drmModeResPtr res = drmModeGetResources(fd_);
//        if (!res) {
//            throw std::runtime_error("Failed to get DRM resources");
//        }
//
//        // 查找连接器
//        drmModeConnectorPtr conn = nullptr;
//        for (int i = 0; i < res->count_connectors; i++) {
//            conn = drmModeGetConnector(fd_, res->connectors[i]);
//            if (conn && conn->connection == DRM_MODE_CONNECTED && conn->count_modes > 0) {
//                connector_id_ = conn->connector_id;
//                break;
//            }
//            if (conn) drmModeFreeConnector(conn);
//            conn = nullptr;
//        }
//
//        if (!conn) {
//            drmModeFreeResources(res);
//            throw std::runtime_error("No active connector found");
//        }
//
//        // 保存可用显示模式
//        for (int i = 0; i < conn->count_modes; i++) {
//            DisplayMode mode;
//            mode.width = conn->modes[i].hdisplay;
//            mode.height = conn->modes[i].vdisplay;
//            mode.refresh_rate = conn->modes[i].vrefresh;
//            mode.mode_info = conn->modes[i];
//            available_modes_.push_back(mode);
//        }
//
//        // 查找编码器
//        drmModeEncoderPtr enc = drmModeGetEncoder(fd_, conn->encoder_id);
//        if (!enc) {
//            drmModeFreeConnector(conn);
//            drmModeFreeResources(res);
//            throw std::runtime_error("Failed to get encoder");
//        }
//
//        crtc_id_ = enc->crtc_id;
//
//        // 尝试使用当前分辨率设置模式
//        bool mode_set = false;
//        for (const auto& mode : available_modes_) {
//            if (mode.width == width_ && mode.height == height_) {
//                set_crtc(mode.mode_info);
//                current_mode_ = mode;
//                mode_set = true;
//                break;
//            }
//        }
//
//        // 如果没有匹配模式，使用第一个可用模式
//        if (!mode_set && !available_modes_.empty()) {
//            set_crtc(available_modes_[0].mode_info);
//            current_mode_ = available_modes_[0];
//            width_ = current_mode_.width;
//            height_ = current_mode_.height;
//        }
//
//        // 释放资源
//        drmModeFreeEncoder(enc);
//        drmModeFreeConnector(conn);
//        drmModeFreeResources(res);
//    }
//
//    // 设置CRTC
//    void set_crtc(const drmModeModeInfo& mode) {
//        int ret = drmModeSetCrtc(fd_, crtc_id_, 0, 0, 0,
//                                 &connector_id_, 1, &mode);
//        if (ret != 0) {
//            throw std::runtime_error("Failed to set CRTC");
//        }
//    }
//
//    // 创建缓冲区
//    void create_buffers(size_t count) {
//        // 清除现有缓冲区
//        for (auto& buffer : buffers_) {
//            destroy_buffer(buffer);
//        }
//        buffers_.clear();
//
//        // 创建新缓冲区
//        for (size_t i = 0; i < count; i++) {
//            buffers_.push_back(create_buffer());
//        }
//    }
//
//    // 重新创建缓冲区（分辨率变化时）
//    void recreate_buffers() {
//        size_t count = buffers_.size();
//        create_buffers(count);
//        current_buffer_ = 0;
//    }
//
//    // 创建单个缓冲区
//    DisplayBuffer create_buffer() {
//        DisplayBuffer buffer = {0};
//
//        // 创建dumb缓冲区
//        drm_mode_create_dumb create_req = {0};
//        create_req.width = width_;
//        create_req.height = height_;
//        create_req.bpp = 8;  // 8位/像素
//
//        if (drmIoctl(fd_, DRM_IOCTL_MODE_CREATE_DUMB, &create_req) < 0) {
//            throw std::runtime_error("Failed to create dumb buffer");
//        }
//
//        buffer.handle = create_req.handle;
//        buffer.pitch = create_req.pitch;
//        buffer.size = create_req.size;
//
//        // 映射缓冲区
//        drm_mode_map_dumb map_req = {0};
//        map_req.handle = buffer.handle;
//        if (drmIoctl(fd_, DRM_IOCTL_MODE_MAP_DUMB, &map_req) < 0) {
//            drm_mode_destroy_dumb destroy_req = {buffer.handle};
//            drmIoctl(fd_, DRM_IOCTL_MODE_DESTROY_DUMB, &destroy_req);
//            throw std::runtime_error("Failed to map dumb buffer");
//        }
//
//        buffer.map = mmap(0, buffer.size, PROT_READ | PROT_WRITE,
//                          MAP_SHARED, fd_, map_req.offset);
//        if (buffer.map == MAP_FAILED) {
//            drm_mode_destroy_dumb destroy_req = {buffer.handle};
//            drmIoctl(fd_, DRM_IOCTL_MODE_DESTROY_DUMB, &destroy_req);
//            throw std::runtime_error("Failed to mmap buffer");
//        }
//
//        // 创建帧缓冲区 (NV12格式)
//        uint32_t handles[2] = {buffer.handle, buffer.handle};
//        uint32_t pitches[2] = {buffer.pitch, buffer.pitch};
//        uint32_t offsets[2] = {0, height_ * buffer.pitch}; // UV平面偏移
//
//        if (drmModeAddFB2(fd_, width_, height_, DRM_FORMAT_NV12,
//                          handles, pitches, offsets, &buffer.fb_id, 0) != 0) {
//            munmap(buffer.map, buffer.size);
//            drm_mode_destroy_dumb destroy_req = {buffer.handle};
//            drmIoctl(fd_, DRM_IOCTL_MODE_DESTROY_DUMB, &destroy_req);
//            throw std::runtime_error("Failed to add framebuffer");
//        }
//
//        return buffer;
//    }
//
//    // 销毁单个缓冲区
//    void destroy_buffer(DisplayBuffer& buffer) {
//        if (buffer.fb_id) {
//            drmModeRmFB(fd_, buffer.fb_id);
//            buffer.fb_id = 0;
//        }
//        if (buffer.map) {
//            munmap(buffer.map, buffer.size);
//            buffer.map = nullptr;
//        }
//        if (buffer.handle) {
//            drm_mode_destroy_dumb destroy_req = {buffer.handle};
//            drmIoctl(fd_, DRM_IOCTL_MODE_DESTROY_DUMB, &destroy_req);
//            buffer.handle = 0;
//        }
//    }
//
//    // 清理资源
//    void cleanup() {
//        for (auto& buffer : buffers_) {
//            destroy_buffer(buffer);
//        }
//        buffers_.clear();
//
//        if (fd_ >= 0) {
//            close(fd_);
//            fd_ = -1;
//        }
//    }
//
//    // 禁用拷贝和赋值
//    DrmDisplay(const DrmDisplay&) = delete;
//    DrmDisplay& operator=(const DrmDisplay&) = delete;
//};
//#endif //RK3588_PROJ_DISPLAY_H
