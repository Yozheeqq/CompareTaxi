#!/bin/bash

VERSION=1.21.0
URL="https://github.com/microsoft/onnxruntime/releases/download/v$VERSION/onnxruntime-linux-x64-$VERSION.tgz"

echo "Downloading ONNX Runtime"
wget -q --show-progress "$URL" -O onnxruntime.tgz

tar -xzf onnxruntime.tgz
rm onnxruntime.tgz

echo "ONNX runtime was downloaded"
