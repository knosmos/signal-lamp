import socketio

sio = socketio.Client()
name = input("name > ")

@sio.event
def connect():
    print('connection established')

@sio.event
def disconnect():
    print('disconnected from server')

@sio.on("state_update")
def state_update(data):
    pass
    #print(data)

sio.connect('http://localhost:7777')
while True:
    s = input("state > ")
    sio.emit("b", {"sender": name, "msg": s})