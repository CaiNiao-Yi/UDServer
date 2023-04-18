# UDServer

UDServer是一个基于Crow库和C++17的文件传输服务器。

## 简介

UDServer支持GET和POST请求。GET请求用于下载指定名称的文件，POST请求用于上传文件。服务器可通过命令行指定端口号，若未指定则默认使用8000端口。

## 使用方法

### 下载文件

向`/files`发送GET请求，并在URL参数中指定要下载的文件名(filename)。

```http
GET /files?filename=file.txt HTTP/1.1
Host: 127.0.0.1:8000
```

如果文件存在，则服务器会将其作为二进制流返回，并设置相应的Content-Type和Content-Disposition响应头以提示浏览器下载该文件。如果文件不存在，则服务器会返回状态码404和"File Not Found"消息体。

### 上传文件

向`/files`发送POST请求，消息体中携带至少一个文件数据。文件数据通过multipart/form-data格式传输。

```http
POST /files HTTP/1.1
Host: 127.0.0.1:8000
Content-Type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW

------WebKitFormBoundary7MA4YWxkTrZu0gW
Content-Disposition: form-data; name="file"; filename="example.txt"
Content-Type: text/plain

This is an example file.

------WebKitFormBoundary7MA4YWxkTrZu0gW--
```

如果接收到的文件数据为空，则服务器会返回状态码400和"Empty Files"消息体。如果接收到的文件数据不为空，则服务器会将其保存到当前目录下，并返回状态码200和"File Saved"消息体。

## 命令行参数

UDServer支持以下命令行参数：

| 参数 | 说明 | 默认值 |
| --- | --- | --- |
| \<port\> | 指定监听的端口号 | 8000 |
## 编写目的

UDServer程序的主要目的是为了方便测试微信小程序的上传和下载功能。通过该程序，可以模拟服务器端的文件传输操作，便于测试微信小程序的前后端交互逻辑。

## 微信小程序使用说明

在微信小程序中，可以通过wx.request函数发送GET和POST请求以与UDServer进行交互。

### 下载文件

通过wx.downloadFile函数向UDServer发送GET请求，并在URL参数中指定要下载的文件名(filename)。

```javascript
wx.downloadFile({
  url: 'http://127.0.0.1:8000/files?filename=file.txt',
  success: function(res) {
    // 下载成功后，res.tempFilePath为临时文件路径
    console.log(res.tempFilePath);
  },
  fail: function(err) {
    console.log(err);
  }
})
```

### 上传文件

通过wx.uploadFile函数向UDServer发送POST请求，消息体中携带至少一个文件数据。文件数据通过multipart/form-data格式传输。

```javascript
wx.chooseMedia({
  success: function(res) {
    wx.uploadFile({
      url: 'http://127.0.0.1:8000/files',
      filePath: res.tempFilePaths[0],
      name: 'file',
      success: function(res) {
        console.log(res.data);
      },
      fail: function(err) {
        console.log(err);
      }
    })
  },
  fail: function(err) {
    console.log(err);
  }
})
```

其中，wx.chooseImage函数用于选择要上传的图片文件。上传成功后，UDServer会将其保存到当前目录下，并返回"File Saved"消息体。

## 构建方式

UDServer程序需要使用xmake和vcpkg来进行构建，具体步骤如下：

### 使用xmake构建

1. 安装xmake

   可以通过官方网站(https://xmake.io/)或以下命令安装：

   ```bash
   curl -fsSL https://xmake.io/shget.text | bash
   ```
2. 安装vcpkg

   可以通过以下命令克隆项目并安装：

   ```bash
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   ./bootstrap-vcpkg.sh
   ./vcpkg integrate install
   ```
3. 执行以下命令进行构建：

   ```bash
   xmake
   ```

4. 构建成功后，可在build目录下找到生成的可执行文件。
