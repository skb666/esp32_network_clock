import paho.mqtt.client as mqtt


def on_message(client, userdata, msg):
    '''处理message回调'''
    print('topic: {}'.format(msg.topic))
    print('message: {}'.format(str(msg.payload)))


HOST_IP = 'b-b.icu'  # Server的IP地址
HOST_PORT = 1883  # mosquitto 默认打开端口
TOPIC_ID1 = '/esp32_project/event'
TOPIC_ID2 = '/esp32_project/data'
# 建立一个MQTT的客户端
client = mqtt.Client("pc_sub")
client.username_pw_set('user', 'user_password')
# 绑定数据接收回调函数
client.on_message = on_message

# 连接MQTT服务器
client.connect(HOST_IP, HOST_PORT, 60)
# 订阅主题
client.subscribe(TOPIC_ID1, 1)
client.subscribe(TOPIC_ID2, 1)
client.subscribe("/esp32_project/control", 2)

# 阻塞式， 循环往复，一直处理网络数据，断开重连
client.loop_forever()
