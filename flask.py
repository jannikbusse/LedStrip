from flask import Flask
import socket

UDP_IP = "192.168.2.65"
UDP_PORT = 1234
sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
app = Flask(__name__)


@app.route("/on")
def hello_world():
    sock.sendto(b"S", (UDP_IP, UDP_PORT))
    return f"setOn"

@app.route("/clearcolors")
def clear_cols():
    sock.sendto(b"C", (UDP_IP, UDP_PORT))
    return f"clearedColors"

@app.route("/brightness/<on>")
def brightness(on):
    str = f"Brightness{on}"
    sock.sendto(bytes(str, "utf-8"), (UDP_IP, UDP_PORT))
    return f"Hellu {on}"

@app.route("/speed/<on>")
def speed(on):
    str = f"O{on}"
    sock.sendto(bytes(str, "utf-8"), (UDP_IP, UDP_PORT))
    return f"set speed to {on}"

@app.route("/addcolor/<col>")
def add_color(col):
    str = f"A{col}"
    sock.sendto(bytes(str, "utf-8"), (UDP_IP, UDP_PORT))
    return f"added color{col}"

@app.route("/timer/<time>")
def set_timer(time):
    str = f"T{time}"
    sock.sendto(bytes(str, "utf-8"), (UDP_IP, UDP_PORT))
    return f"set timer{time}"

@app.route("/flicker")
def flicker():
    sock.sendto(b"F", (UDP_IP, UDP_PORT))
    return f"flicker switch"


