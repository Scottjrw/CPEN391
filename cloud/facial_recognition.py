#!flask/bin/python
import face_recognition
import json
import os
from flask import Flask, jsonify, make_response, abort, request, redirect, url_for, session
from flask_uploads import UploadSet, configure_uploads, IMAGES
from werkzeug import secure_filename
from os.path import expanduser
from db import *
from hashlib import md5
import numpy as np
import requests

home = expanduser("~")

app = Flask(__name__)

UPLOAD_FOLDER = 'uploads'
ALLOWED_EXTENSIONS = set(['txt', 'pdf', 'png', 'jpg', 'jpeg', 'gif'])

app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER

def allowed_file(filename):
    return '.' in filename and \
           filename.rsplit('.', 1)[1].lower() in ALLOWED_EXTENSIONS

@app.before_request
def before_request():
    db.connect()


@app.after_request
def after_request(response):
    db.close()
    return response


@app.route('/')
def hello_world():
    return 'Hello, World!'

def auth_user(user):
    session['logged_in'] = True
    session['user_id'] = user.id
    session['username'] = user.username
    print('You are logged in as %s' % (user.username))

def get_current_user():
    if session.get('logged_in'):
        return Users.get(Users.id == session['user_id'])


@app.route('/join', methods=['GET', 'POST'])
def addUser():
	if request.method == 'POST' and request.form['username']:
		# check if the post request has the file part
		if 'file' not in request.files:
			print('No file part')
			return redirect(request.url)
		file = request.files['file']
		# if user does not select file, browser also
		# submit a empty part without filename
		if file.filename == '':
			print('No selected file')
			return redirect(request.url)
		if file and allowed_file(file.filename):
			picture = face_recognition.load_image_file(file)
			face_encoding = face_recognition.face_encodings(picture)[0]

			try:
				with db.atomic():
					# Attempt to create the user. If the username is taken, due to the
					# unique constraint, the database will raise an IntegrityError.
					user = Users.create(
						username=request.form['username'],
						password=md5((request.form['password']).encode('utf-8')).hexdigest(),
						photo_encoding=face_encoding.tostring(),
						ifttt_requests="")

				# mark the user as being 'authenticated' by setting the session vars
				auth_user(user)
				return 'User joined.'

			except IntegrityError:
				print('That username is already taken')
			return 'User failed to join.'


@app.route('/loginByFace', methods=['GET', 'POST'])    
def loginByFace():
	if request.method == 'POST':
		# check if the post request has the file part
		if 'file' not in request.files:
			print('No file part')
			return redirect(request.url)
		file = request.files['file']
		# if user does not select file, browser also
		# submit a empty part without filename
		if file.filename == '':
			print('No selected file')
			return redirect(request.url)
		if file and allowed_file(file.filename):
			picture_of_me = face_recognition.load_image_file(file)
			my_face_encoding = face_recognition.face_encodings(picture_of_me)[0]

			# my_face_encoding now contains a universal 'encoding' of my facial features that can be compared to any other picture of a face!

			cursor = db.cursor()
			cursor.execute("SELECT username, photo_encoding FROM users")
			result_set = cursor.fetchall()
			for row in result_set:
				encoding = np.fromstring(row[1], dtype=my_face_encoding[0].dtype)
				results = face_recognition.compare_faces([my_face_encoding], encoding)
				if results[0] == True:
					user = Users.get(Users.username == row[0])
					auth_user(user)
					return 'It is a picture of ' + str(row[0])
				else:
					print ('It is not a picture of ' + str(row[0]))

			return 'No user found.'


@app.route('/loginByPassword', methods=['GET', 'POST'])    
def loginByPassword():
	if request.method == 'POST':

		r = request.get_json()
		print(r.get('username'))
		print(r.get('password'))

		username=r.get('username')

		password=md5((r.get('password')).encode('utf-8')).hexdigest()

		# my_face_encoding now contains a universal 'encoding' of my facial features that can be compared to any other picture of a face!

		try:
			user = Users.get((Users.username == username) & (Users.password == password))
		except Users.DoesNotExist:
			print('The password entered is incorrect')
		else:
			auth_user(user)
			return 'Logged in as ' + str(username)


		cursor = db.cursor()
		cursor.execute("SELECT username, password FROM users")
		result_set = cursor.fetchall()
		for row in result_set:
			if (row[0] == username) and (row[1] == password):
				auth_user(user)
				return 'Logged in as ' + str(row[0])

		return 'No user found.'


@app.route('/addApplet', methods=['POST'])
def addApplet():
	if request.method == 'POST':
		username = get_current_user()

		cursor = db.cursor()
		cursor.execute("SELECT id, username FROM users")
		result_set = cursor.fetchall()
		for row in result_set:
			if (row[1] == username):
				user_id = row[0]
				cursor.execute("UPDATE users SET ifttt_requests = ? WHERE id = ?", (request.form['applet'], user_id))

		cursor.execute("SELECT ifttt_requests, username FROM users")
		result_set = cursor.fetchall()
		for row in result_set:
			if (row[1] == username):
				print(str(row[0]))

		return 'Added applet'




@app.route('/applet', methods=['POST'])
def applet():
	if request.method == 'POST':
	    # report = {}
	    # report["value1"] = request.form['value1']
	    # report["value2"] = request.form['value2']
	    # report["value3"] = request.form['value3']
		request_name = "https://maker.ifttt.com/trigger/{applet}/with/key/egyN_jF6pzR88s9b8rFg0jTYXbbIpEGH-rB_zGobz_i".format(applet=request.form['applet'])
		requests.post(request_name)
		return 'applet successful'


app.config['SESSION_TYPE'] = 'filesystem'
app.config['SECRET_KEY'] = 'reds209ndsldssdsljdsldsdsljdsldksdksdsdfsfsfsfis'

if __name__ == '__main__':
	app.run(host='0.0.0.0', port=6000)