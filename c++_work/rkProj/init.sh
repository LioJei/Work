#!/bin/sh

## Brief:   初始化程序脚本，添加到服务，开启自启和重启
## Author:  lijunjie
## Date:    2025/4/15
## Copyright (c) 2025 cdly.Co.,Ltd. All rights reserved.

echo "Starting initial..."
## 定义变量
# 定义程序存放位置
EXECDIR="/home/lio/software/rkProj"
# 定义工作目录
WORKDIR="/home/lio/software/"
# 定义要创建的文件名
SERVFILE="/etc/systemd/system/rk_initial.service"
# 使用touch命令创建文件
sudo touch "$SERVFILE"
if [ -f "$SERVFILE" ]; then
    echo "Server file <$SERVFILE> add succeed."
else
    echo "Server file <$SERVFILE> add failed."
fi
# 初始化服务内容
echo "Initial Server config..."
sudo cat  << EOF > $SERVFILE
[Unit]
Description=RkProj Daemon Service
After=network.target
[Service]
ExecStart=$EXECDIR
Restart=always
RestartSec=3
User=lio
WorkingDirectory=$WORKDIR
[Install]
WantedBy=multi-lio.target
EOF
# 重新加载systemd配置
echo "Restart System Server."
sudo systemctl daemon-reload
# 启动服务
echo "Start My Server."
sudo systemctl start rk_initial
# 设置开机自启
echo "Set My Server PowerBoot."
sudo systemctl enable rk_initial
# 关闭服务< sudo systemctl stop rk_initial >
# 关闭自启< systemctl disable rk_initial >



