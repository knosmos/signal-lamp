'''
import asyncio
import socketio

sio = socketio.AsyncClient()
name = input("name > ")

@sio.event
async def connect():
    print('connection established')
    await sio.emit("b", {"sender": name, "msg": "hello world!"})

@sio.event
async def state_update(data):
    print('message received with ', data)
    await sio.emit('my response', {'response': 'my response'})

@sio.event
async def disconnect():
    print('disconnected from server')

async def main():
    await sio.connect('http://localhost:7777/')
    while True:
        s = input("state > ")
        await sio.emit("b", {"sender": name, "msg": s})
        print("sent!")

if __name__ == '__main__':
    asyncio.run(main())
'''

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