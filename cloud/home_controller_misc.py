@app.route('/joinByPicture', methods=['GET', 'POST', 'OPTIONS'])    
# @crossdomain(origin='*')
def joinByPicture():
	if request.method == 'POST' or request.method == 'OPTIONS':
		# check if the post request has the file part
		print(request.form['username'])
		print(request.form['password'])
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
			print("face encoding done")

			try:
				with db.atomic():
					# Attempt to create the user. If the username is taken, due to the
					# unique constraint, the database will raise an IntegrityError.
					user = Users.create(
						username=request.form['username'],
						password=md5((request.form['password']).encode('utf-8')).hexdigest(),
						photo_encoding=face_encoding.tostring())

				# mark the user as being 'authenticated' by setting the session vars
				auth_user(user)
				return request.form['username']


			except IntegrityError:
				print('That username is already taken')
				return 'Username Taken'
			return 'Fail'


@app.route('/loginByFacePicture', methods=['GET', 'POST'])    
def loginByFacePicture():
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

			try:
				my_face_encoding = face_recognition.face_encodings(picture_of_me)[0]
			except:
				print('no face appeared')
				return 'login failed'

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
					return str(row[0])

			return 'Fail'

