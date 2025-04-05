#!/bin/bash

echo "Installing system dependencies..."
DEPS_FILE="https://raw.githubusercontent.com/userver-framework/userver/refs/heads/develop/scripts/docs/en/deps/ubuntu-24.04.md" && \
sudo apt update && \
sudo apt install --allow-downgrades -y $(wget -q -O - ${DEPS_FILE}) && \
sudo apt install -y make cmake clang-format clang clang-tools lldb ninja-build \
                   protobuf-compiler python3-protobuf python3 python3-dev python3-pip python3-venv

echo "Downloading ONNX Runtime"
VERSION=1.21.0
URL="https://github.com/microsoft/onnxruntime/releases/download/v$VERSION/onnxruntime-linux-x64-$VERSION.tgz"
wget -q --show-progress "$URL" -O onnxruntime.tgz

tar -xzf onnxruntime.tgz
rm onnxruntime.tgz
mv onnxruntime-linux-x64-$VERSION onnxruntime
echo "ONNX runtime was downloaded and extracted"

# Установка Kafka
echo "Installing Kafka..."
wget -c https://dlcdn.apache.org/kafka/3.9.0/kafka_2.13-3.9.0.tgz -O kafka.tgz
tar -xzf kafka.tgz
sudo mkdir -p /etc/kafka
sudo mv kafka_2.13-3.9.0 /etc/kafka
rm kafka.tgz
echo "Kafka installed to /etc/kafka"

echo "Installing Java..."
sudo apt update && sudo apt install -y openjdk-11-jre
echo 'export JAVA_HOME=/usr/lib/jvm/java-11-openjdk-amd64' >> ~/.bashrc
source ~/.bashrc
echo "Java installed:"
java -version
