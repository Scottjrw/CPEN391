#!flask/bin/python
import face_recognition
import json
import os
from flask import Flask, jsonify, make_response, abort, request, redirect, url_for
from flask_uploads import UploadSet, configure_uploads, IMAGES
from werkzeug import secure_filename
from os.path import expanduser
from db import *
from hashlib import md5

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

# @app.route('/upload', methods=['GET', 'POST'])
# def upload():
# 	if request.method == 'POST':
# 		# check if the post request has the file part
# 		if 'file' not in request.files:
# 			print('No file part')
# 			return redirect(request.url)
# 		file = request.files['file']
# 		# if user does not select file, browser also
# 		# submit a empty part without filename
# 		if file.filename == '':
# 			print('No selected file')
# 			return redirect(request.url)
# 		if file and allowed_file(file.filename):
# 			picture = face_recognition.load_image_file(file)
# 			face_encoding = face_recognition.face_encodings(picture)[0]

# 			text_file = open("Output.txt", "wb")
# 			text_file.write(face_encoding.tostring())
# 			text_file.close()
# 			return 'New Person in front of screen.'


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
						photo_encoding=face_encoding.tostring())

				# mark the user as being 'authenticated' by setting the session vars
				# auth_user(user)
				return 'User joined.'

			except IntegrityError:
				print('That username is already taken')
			return 'User failed to join.'


@app.route('/detect', methods=['GET', 'POST'])    
def detect():
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
			picture_of_me = face_recognition.load_image_file("known.jpg")
			my_face_encoding = face_recognition.face_encodings(picture_of_me)[0]

			# my_face_encoding now contains a universal 'encoding' of my facial features that can be compared to any other picture of a face!

			cursor = db.cursor()
			cursor.execute("SELECT id, photo_encoding FROM users")
			result_set = cursor.fetchall()
			for row in result_set:
				encoding = np.fromstring(row["photo_encoding"], dtype=unknown_face_encodings[0].dtype)
				results = face_recognition.compare_faces([my_face_encoding], encoding)
				if results[0] == True:
				    return 'It is a picture of ' + row["id"].tostring()
				else:
					print 'It is not a picture of ' + row["id"].tostring()

			return 'No user found.'



if __name__ == '__main__':
    app.run(host='0.0.0.0', port=6000)