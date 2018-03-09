#!flask/bin/python
import face_recognition
import json
import os
from flask import Flask, jsonify, make_response, abort, request, redirect, url_for
from flask_uploads import UploadSet, configure_uploads, IMAGES
from werkzeug import secure_filename

app = Flask(__name__)

UPLOAD_FOLDER = 'uploads'
ALLOWED_EXTENSIONS = set(['txt', 'pdf', 'png', 'jpg', 'jpeg', 'gif'])

app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER

def allowed_file(filename):
    return '.' in filename and \
           filename.rsplit('.', 1)[1].lower() in ALLOWED_EXTENSIONS

@app.route('/')
def hello_world():
    return 'Hello, World!'

@app.route('/upload', methods=['GET', 'POST'])
def upload():
	if request.method == 'POST':
		# check if the post request has the file part
		if 'file' not in request.files:
			flash('No file part')
			return redirect(request.url)
		file = request.files['file']
		# if user does not select file, browser also
		# submit a empty part without filename
		if file.filename == '':
			flash('No selected file')
			return redirect(request.url)
		if file and allowed_file(file.filename):
			filename = secure_filename(file.filename)
			file.save(os.path.join(app.config['UPLOAD_FOLDER'], filename))
			return redirect(url_for('upload', filename='uknownUser.jpg'))
	return '''
	<!doctype html>
	<title>Upload new File</title>
	<h1>Upload new File</h1>
	<form method=post enctype=multipart/form-data>
	  <p><input type=file name=file>
		 <input type=submit value=Upload>
	</form>
	'''


@app.route('/detect', methods=['GET'])    
def detect():
	picture_of_me = face_recognition.load_image_file("known.jpg")
	my_face_encoding = face_recognition.face_encodings(picture_of_me)[0]

	# my_face_encoding now contains a universal 'encoding' of my facial features that can be compared to any other picture of a face!

	unknown_picture = face_recognition.load_image_file("uploads/uknownUser.jpg")
	unknown_face_encoding = face_recognition.face_encodings(unknown_picture)[0]

	# Now we can see the two face encodings are of the same person with `compare_faces`!

	results = face_recognition.compare_faces([my_face_encoding], unknown_face_encoding)

	if results[0] == True:
	    print("It's a picture of me!")
	else:
	    print("It's not a picture of me!")



if __name__ == '__main__':
    app.run(host='0.0.0.0', port=6000)