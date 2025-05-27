#!/bin/sh

## Brief:   初始化程序脚本，添加到服务，开启自启和重启
## Author:  lijunjie
## Date:    2025/4/15
## Copyright (c) 2025 CHENGDU LIYANG INFORMATION TECHNOLOGYCo.,Ltd. All rights reserved.

echo "Starting initial..."
# 定义程序存放位置
EXECDIR="/home/lio/software/rkProj"
# 定义工作目录
WORKDIR="/home/lio/software/"
# 定义要创建的文件名
SERVFILE="/etc/systemd/system/rk_initial.service"
# 初始化服务内容
echo "Initial Server config..."
# shellcheck disable=SC2024
sudo cat << EOF > "$SERVFILE" || { echo "Failed to create server file <$SERVFILE>."; exit 1; }
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

if [ $? -eq 0 ]; then
    echo "Server file <$SERVFILE> add succeed."
else
    echo "Server file <$SERVFILE> add failed."
    exit 1
fi
# 重新加载systemd配置
echo "Restart System Server."
sudo systemctl daemon-reload || { echo "Failed to reload systemd daemon."; exit 1; }
# 启动服务
echo "Start My Server."
sudo systemctl start rk_initial || { echo "Failed to start rk_initial service."; exit 1; }
# 设置开机自启
echo "Set My Server PowerBoot."
sudo systemctl enable rk_initial || { echo "Failed to enable rk_initial service."; exit 1; }
# 关闭服务< sudo systemctl stop rk_initial >
# 关闭自启< systemctl disable rk_initial >



