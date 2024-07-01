# Thinking in Qt

经营淘宝店 cuteqt.taobao.com 一年有余，看过很多同学的 Qt 代码，有一些想法，想记录一下。

第一篇想讲一下 Qt 程序的架构

一般的，在数据处理流程中，通常会经历以下几个步骤：

1. 数据协议解析（如以太网、串口、Wi-Fi、蓝牙、USB、CAN等）
2. 后处理（如数据清洗、过滤和转换、数据聚合、数据压缩、加密和解密、特征提取、信号处理、图像处理、视频处理、统计分析、模式识别、自然语言处理和时间序列分析等）
3. 界面渲染。

界面渲染通常在主线程上进行，而数据解析和后处理则在各自独立的线程中运行。

![1](1.svg)

接下来，讲一下 Qt 如何创建多线程

让我们回顾下 Qt 文档中的示例代码

第一种是继承 QThread

```cpp
class WorkerThread : public QThread
{
    Q_OBJECT
    void run() override {
        QString result;
        /* ... here is the expensive or blocking operation ... */
        emit resultReady(result);
    }
signals:
    void resultReady(const QString &s);
};
void MyObject::startWorkInAThread()
{
    WorkerThread *workerThread = new WorkerThread(this);
    connect(workerThread, &WorkerThread::resultReady, this, &MyObject::handleResults);
    connect(workerThread, &WorkerThread::finished, workerThread, &QObject::deleteLater);
    workerThread->start();
}
```

第二种是 moveToThread

```cpp
class Worker : public QObject
{
    Q_OBJECT
public slots:
    void doWork(const QString &parameter) {
        QString result;
        /* ... here is the expensive or blocking operation ... */
        emit resultReady(result);
    }
signals:
    void resultReady(const QString &result);
};
class Controller : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    Controller() {
        Worker *worker = new Worker;
        worker->moveToThread(&workerThread);
        connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
        connect(this, &Controller::operate, worker, &Worker::doWork);
        connect(worker, &Worker::resultReady, this, &Controller::handleResults);
        workerThread.start();
    }
    ~Controller() {
        workerThread.quit();
        workerThread.wait();
    }
public slots:
    void handleResults(const QString &);
signals:
    void operate(const QString &);
};
```

两种写法都有各自应用的场景

第一种写法的应用场景：你只想在子线程里做一件简单的事时。比如不停地接收串口的数据或者创建一个二进制文件。

第二种写法的应用场景：你想异步操作一个硬件，不阻塞 UI。比如调用摄像头 SDK 的各种 API。

一般来说，第二种场景更常见一些，而且第一种场景也可以用第二种写法改写。

这里为了演示，我选了一个我熟悉的场景，相机的使用。

相机作为 Server 接收 PC 上 Client 的 TCP 消息，可以被修改相机参数，也可以主动发送图像数据

为了简化，我们直接用 QTcpServer 写个软件，模拟相机硬件，然后再用 QTcpSocket 写个 PC 客户端软件，控制相机。

然后我们基于 Tcp 设计一个简单的自定义通信协议

![](2.svg)

这是消息头的定义：

第 1 字节为固定值 0x27

第 2 字节为消息类型，0x01 表示 **设置参数** 0x02 代表 **获取参数** 0x03 代表 **获取图像**

第 3 4 两字节为消息 ID，从 0 开始单调递增

第 5 6 7 8 四字节为后面要接收的消息体（也称为负载，即 Payload）的长度



先运行 Server 再运行 Client，截图如下
![image-20240618181448776](image-20240618181448776.png)



Server 和 Client 处理数据的逻辑是很相似的。

CameraServer::readClient 和 CameraClient::readServer 即服务端和客户端最核心的逻辑。

代码非常浅显，这里就不多赘述。

我们主要来看下客户端的多线程部分。

这里的 MainWindow 就相当于 Controller，CameraClient 就相当于 Worker

注意，CameraClient 在 new 的时候，不要这样写：m_camera_client = new CameraClient(this);

应该这样，构造函数参数为空：m_camera_client = new CameraClient;

然后不要忘记在 MainWindow 的析构里添加 quit 和 wait

注意，先 quit 再 wait，这样才是优雅地退出线程的写法

最后，也是最重要的，关于函数调用的方式

比如我在主线程（即 MainWindow 所在线程），想通过子线程发送命令给服务端

我们不能通过指针 m_camera_client 直接调用 requestImage 函数

而应该先在 new 完 m_camera_client 的下一行（即 MainWindow 的构造函数），

写 connect(this, &MainWindow::requestImage, m_camera_client, &CameraClient::requestImage); 类似的代码

注意这个 this，一般非多线程的情况，我们很少让 this 发信号是吧 (#^.^#)

然后在 MainWindow 想调用 requestImage 函数的地方（我这里是 pushButton_2 按钮点击时），emit 一个信号，

当然，这个信号肯定要在 MainWindow 的头文件里用 signals 声明一下

这样对应的槽函数就会在子线程的队列中依次执行，实现多线程之间异步的通信。

需要强调的是，虽然 m_camera_client 已经 moveToThread 了，

但如果你直接通过指针访问 m_camera_client 的函数，它是在主线程运行的，

只有通过信号槽的方式，函数才会在子线程运行。

当然，原先 public 的函数，要改成 public slots 的槽函数

 

Qt 的多线程写起来很简单，啰嗦了这么多，主要是因为屡次看到别人写的稀奇古怪的 Qt 代码

比如在 run 里 new 一个 QTcpSocket，再加一个 exec 阻塞。

比如使用 event2/event.h 去写 callback 或者 用 std::thread + std::function 去写回调函数。

比如 this 发信号后再用 this 接收信号。

本质上都是对 Qt 信号槽以及 QThread 不够了解。

 
