import serial
from flask import Flask, render_template, request

app = Flask(__name__)

arduino = serial.Serial("/dev/ttyACM0", 9600, timeout=1)


@app.route("/")
def index():
    return render_template("index.html")


@app.route("/power", methods=["POST"])
def power():
    state = request.form.get("state")
    if state == "on":
        arduino.write(b"O")
    elif state == "off":
        arduino.write(b"F")
    return ("", 204)


@app.route("/brightness", methods=["POST"])
def brightness():
    level = request.form.get("brightness", "50")  # default '50' if empty
    if level.isdigit():  # is input valid
        level = int(level)
        if 0 <= level <= 100:  # acceptable range
            # scale brightness to Arduino's range (0-255)
            scaled_level = int(level * 255 / 100)
            arduino.write(
                f"B{scaled_level}\n".encode()
            )  # send 'B<scaled_level>' to Arduino
            return ("", 204)
        else:
            return "Brightness must be between 0 and 100.", 400
    else:
        return "Invalid brightness value.", 400


@app.route("/pattern", methods=["POST"])
def pattern():
    pattern = request.form.get("pattern")
    color = request.form.get("color")  # color in R,G,B format
    if color:
        arduino.write(f"C{color}\n".encode())  # send color before changing pattern
    arduino.write(f"P{pattern}\n".encode())  # send 'P<pattern>' to set LED pattern
    return ("", 204)


@app.route("/speed", methods=["POST"])
def speed():
    speed = request.form.get("speed", "50")  # default speed 50 if empty
    if speed.isdigit():
        speed = int(speed)
        if 1 <= speed <= 100:  # acceptable range
            arduino.write(f"S{speed}\n".encode())  # send 'S<speed>' to Arduino
            return ("", 204)
        else:
            return "Speed must be between 1 and 100.", 400
    else:
        return "Invalid speed value.", 400


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
