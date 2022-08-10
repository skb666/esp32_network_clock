import paho.mqtt.client as mqtt
import time

HOST_IP = 'b-b.icu'  # Server的IP地址
HOST_PORT = 1883  # mosquitto 默认打开端口
TOPIC_ID = '/esp32_project/control'  # TOPIC的ID

# 建立一个MQTT的客户端
client = mqtt.Client("pc_pub")
client.username_pw_set('user', 'user_password')
# 连接到服务器（本机）
client.connect(HOST_IP, HOST_PORT, 60)

count = 0
while True:
    count += 1
    # 待发送的数据
    message = 'MOVE FRORWORD,{}'.format(count)
    # 通过mqtt协议发布数据给server
    client.publish(TOPIC_ID, message)
    # 打印日志
    print('SEND: {}'.format(message))
    # 延时1s
    time.sleep(1)
