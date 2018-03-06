#!flask/bin/python
import face_recognition
import json
from flask import Flask, jsonify, make_response, abort, request
from playhouse.shortcuts import model_to_dict, dict_to_model
from flask.ext.uploads import UploadSet, configure_uploads, IMAGES

app = Flask(__name__)

photos = UploadSet('photos', IMAGES)

app.config['UPLOADED_PHOTOS_DEST'] = 'static/img'
configure_uploads(app, photos)

@app.route('/')
def hello_world():
    return 'Hello, World!'

@app.route('/upload', methods=['GET', 'POST'])
def upload():
    if request.method == 'POST' and 'photo' in request.files:
        filename = photos.save(request.files['photo'])
        return filename


@app.route('/detect', methods=['GET'])    
def detect():
	picture_of_me = face_recognition.load_image_file("known.jpg")
	my_face_encoding = face_recognition.face_encodings(picture_of_me)[0]

	# my_face_encoding now contains a universal 'encoding' of my facial features that can be compared to any other picture of a face!

	unknown_picture = face_recognition.load_image_file("justinbieber.jpeg")
	unknown_face_encoding = face_recognition.face_encodings(unknown_picture)[0]

	# Now we can see the two face encodings are of the same person with `compare_faces`!

	results = face_recognition.compare_faces([my_face_encoding], unknown_face_encoding)

	if results[0] == True:
	    print("It's a picture of me!")
	else:
	    print("It's not a picture of me!")



if __name__ == '__main__':
    app.run(host='0.0.0.0', port=1000)