#!flask/bin/python
import face_recognition
import json
import os
import io
import sys
from flask import Flask, jsonify, make_response, abort, request, redirect, url_for, session, current_app
from flask_uploads import UploadSet, configure_uploads, IMAGES
from werkzeug import secure_filename
from os.path import expanduser
from db import *
from hashlib import md5
import numpy as np
import requests
from PIL import Image
from datetime import timedelta
from functools import update_wrapper
from flask_cors import CORS
from home_controller_misc import *
from home_controller_multi import *
from home_controller_de1 import *
from home_controller_website import *

home = expanduser("~")

app = Flask(__name__)
CORS(app)

UPLOAD_FOLDER = 'uploads'
ALLOWED_EXTENSIONS = set(['txt', 'pdf', 'png', 'jpg', 'jpeg', 'gif'])

app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER


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



if __name__ == '__main__':
	app.secret_key = "reds209ndsldssdsljdsldsdsljdsldksdksdsdfsfsfsfis"
	app.run(host='0.0.0.0', port=6000)
	