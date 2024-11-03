import uvicorn
import socketio
from fastapi import FastAPI

state = {}

#Fast API application
app = FastAPI()

#Socket io (sio) create a Socket.IO server
sio = socketio.AsyncServer(cors_allowed_origins='*', async_mode='asgi')

#wrap with ASGI application
socket_app = socketio.ASGIApp(sio)
app.mount("/", socket_app)

@app.get("/")
async def root():
    return state

@sio.on("connect")
async def connect(sid, env):
    print("client connected: " + str(sid))

@sio.on("disconnect")
async def disconnect(sid):
    print("client disconnected: " + str(sid))

@sio.on("b")
async def broadcast(sid, message):
    print(sid)
    sender, position = message["sender"], message["msg"]
    state[sender] = position
    print(state)
    await sio.emit("state_update", state)

if __name__=="__main__":
    uvicorn.run("server:app", host="0.0.0.0", port=7777, lifespan="on", reload=True)