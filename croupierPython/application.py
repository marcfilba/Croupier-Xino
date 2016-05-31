
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

players = 3

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
app.config['DEBUG'] = True

#turn the flask app into a socketio app
socketio = SocketIO(app)

thread = Thread()
thread_stop_event = Event()

class SerialThread(Thread):
    def __init__(self):
        self.delay = 0.5
        super(SerialThread, self).__init__()

        ser.write('s')
        print 'Start'

    def getDataFromSerial (self):
        while not thread_stop_event.isSet():
            rawData = ''
            while len(rawData.split(',')) != players:
                print rawData
                rawData = ser.readline().rstrip()
                #rawData = '1,2,3,4,5,6'
            socketio.emit('newMsg', {'data': rawData, 'players': players}, namespace='/test')
            sleep(self.delay)

    def run(self):
        self.getDataFromSerial()


@app.route('/')
def index():
    #players = ser.readline().rstrip()	# llegim accel, gyro i temp
    return render_template ('index' + str(players) + '.html')


@socketio.on('connect', namespace='/test')
def test_connect():
    global thread
    print ('Client connected')

    if not thread.isAlive():
        print "Starting Thread"
        thread = SerialThread()
        thread.start()

@socketio.on('disconnect', namespace='/test')
def test_disconnect():
    print('Client disconnected')

if __name__ == '__main__':
    socketio.run(app, host = '0.0.0.0', port = 5001)
