"""
Demo Flask application to test the operation of Flask with socket.io

Aim is to create a webpage that is constantly updated with random numbers from a background python process.

30th May 2014
"""

# Start with a basic flask app webpage.
from flask.ext.socketio import SocketIO, emit
from flask import Flask, render_template, url_for, copy_current_request_context
from time import sleep
from threading import Thread, Event

import serial
import select

serialPort = '/dev/ttyACM0'
baudRate = 115200

ser = serial.Serial(serialPort, baudRate)

players = 0

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
app.config['DEBUG'] = True

#turn the flask app into a socketio app
socketio = SocketIO(app)

#random number Generator Thread
thread = Thread()
thread_stop_event = Event()

class SerialThread(Thread):
    def __init__(self):
        self.delay = 0.1
        super(SerialThread, self).__init__()

    def getDataFromSerial (self):
        while not thread_stop_event.isSet():
            rawData = ser.readline().rstrip()
            rawDataSplitted = rawData.split (",")
            socketio.emit('newMsg', {'data': rawDataSplitted, 'players': players}, namespace='/test')
	    sleep(self.delay)

    def run(self):
        self.getDataFromSerial()


@app.route('/')
def index():
		players = ser.readline().rstrip()	# llegim accel, gyro i temp
        return render_template('index' + str(players) + '.html')

@socketio.on('connect', namespace='/test')
def test_connect():
    global thread
    print('Client connected')

    if not thread.isAlive():
        print "Starting Thread"
        thread = SerialThread()
        thread.start()

@socketio.on('disconnect', namespace='/test')
def test_disconnect():
    print('Client disconnected')


if __name__ == '__main__':
    socketio.run(app, host = '0.0.0.0')
