from flask import Flask, request, abort
import logging
import datetime

app = Flask(__name__)

log = logging.getLogger('werkzeug')
log.setLevel(logging.ERROR)

@app.route('/trigger', methods=['POST'])
def trigger():
	if request.method == 'POST':
		print(datetime.datetime.now(), request.json)
		return 'success', 200
	else:
		abort(400)


@app.route('/rem', methods=['POST'])
def rem():
	if request.method == 'POST':
		print(datetime.datetime.now(), request.json)
		return 'success', 200
	else:
		abort(400)


@app.route('/variance', methods=['POST'])
def variance():
	if request.method == 'POST':
		#print(datetime.datetime.now(), request.json)
		return 'success', 200
	else:
		abort(400)


@app.route('/quality', methods=['POST'])
def rem():
	if request.method == 'POST':
		print(datetime.datetime.now(), request.json)
		return 'success', 200
	else:
		abort(400)


if __name__ == '__main__':
	app.run(host='localhost', port=5000)