@app.route('/pictureSegment', methods=['POST'])
def pictureSegement():
	if request.method == 'POST':
		r = request.get_json()

		list_of_pixels = list()
		hex_string = r.get('picture_segment')

		cursor = db.cursor()

		temp = cursor.execute('SELECT max(id) FROM tempPicture')
		max_id = temp.fetchone()[0]

		print(max_id)

		if max_id is None:
			tempPicture.create(picture_segment = hex_string)
			return 'string added'
		elif (int(max_id) < 18):
			tempPicture.create(picture_segment = hex_string)
			return 'string added'
		else:
			cursor.execute('''DELETE FROM tempPicture''')
			tempPicture.create(picture_segment = hex_string)
			print('table reset')
			return 'string added and new table'

	return 'string failed'



@app.route('/joinWithSegments', methods=['GET', 'POST'])
def joinWithSegments():
	if request.method == 'POST':
		r = request.get_json()
		hex_string = ""
		cursor = db.cursor()

		if (r.get('hex_string') == "done"):
			cursor.execute("SELECT id,picture_segment FROM tempPicture")
			result_set = cursor.fetchall()
			for row in result_set:
				hex_string = hex_string + str(row[1])

		cursor.execute('''DELETE FROM tempPicture''')
		print('table reset')

		list_of_pixels = list()

		for i, c in enumerate(hex_string):
			if (i <= len(hex_string) - 6):
				if (i % 6 == 0):
					r_value = int(hex_string[i:i+2], 16)
					g_value = int(hex_string[i+2:i+4], 16)
					b_value = int(hex_string[i+4:i+6], 16)
					rgb_tuple = (r_value,g_value,b_value)
					list_of_pixels.append(rgb_tuple)

		im = Image.new('RGB', (80,60))
		im.putdata(list_of_pixels)
		im.save('/home/dchau/img.png')


		# byte_array = bytearray.fromhex(r.get('hex-string'))
		# img = Image.open(io.BytesIO(byte_array))
		# img.save('/home/dchau/img.jpg')

		picture = face_recognition.load_image_file('/home/dchau/img.png')
		try:
			face_encoding = face_recognition.face_encodings(picture)[0]
		except:
			print('no face appeared')
			return 'User failed to join'

		if (r.get('username') == ""):
			print('no username')
			return 'User failed to join'

		if (r.get('password') == ""):
			print('no password')
			return 'User failed to join'


		try:
			with db.atomic():
				# Attempt to create the user. If the username is taken, due to the
				# unique constraint, the database will raise an IntegrityError.
				user = Users.create(
					username=r.get('username'),
					password=md5((r.get('password')).encode('utf-8')).hexdigest(),
					photo_encoding=face_encoding.tostring(),
					ifttt_requests="")

			# mark the user as being 'authenticated' by setting the session vars
			auth_user(user)
			return 'User joined.'

		except IntegrityError:
			print('That username is already taken')
		return 'User failed to join.'




@app.route('/loginByFaceHexSegments', methods=['GET', 'POST'])    
def loginByFaceHexSegments():
	if request.method == 'POST':
		r = request.get_json()

		hex_string = ""

		if (r.get('hex_string') == "done"):
			cursor = db.cursor()
			cursor.execute("SELECT id,picture_segment FROM tempPicture")
			result_set = cursor.fetchall()
			for row in result_set:
				hex_string = hex_string + str(row[1])

			cursor.execute('''DELETE FROM tempPicture''')
			print('table reset')

		list_of_pixels = list()
		print(len(hex_string))

		for i, c in enumerate(hex_string):
			if (i <= len(hex_string) - 6):
				if (i % 6 == 0):
					r_value = int(hex_string[i:i+2], 16)
					g_value = int(hex_string[i+2:i+4], 16)
					b_value = int(hex_string[i+4:i+6], 16)
					rgb_tuple = (r_value,g_value,b_value)
					list_of_pixels.append(rgb_tuple)

		im = Image.new('RGB', (80,60))
		im.putdata(list_of_pixels)
		im.save('/home/dchau/img.png')

		picture_of_me = face_recognition.load_image_file('/home/dchau/img.png')

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
				return 'It is a picture of ' + str(row[0])
			else:
				print ('It is not a picture of ' + str(row[0]))

		return 'No user found.'




@app.route('/loginByPassword', methods=['GET', 'POST', 'OPTIONS'])    
def loginByPassword():
	if request.method == 'POST' or request.method == 'OPTIONS':

		r = request.get_json()

		username=r.get('username')

		password=md5((r.get('password')).encode('utf-8')).hexdigest()

		# my_face_encoding now contains a universal 'encoding' of my facial features that can be compared to any other picture of a face!

		try:
			user = Users.get((Users.username == username) & (Users.password == password))
		except Users.DoesNotExist:
			return 'Fail'
		else:
			auth_user(user)
			return username


		# cursor = db.cursor()
		# cursor.execute("SELECT username, password FROM users")
		# result_set = cursor.fetchall()
		# for row in result_set:
		# 	if (row[0] == username) and (row[1] == password):
		# 		auth_user(user)
		# 		return 'Logged in as ' + str(row[0])

		return 'Fail'




@app.route('/runApplet', methods=['POST'])
def applet():
	if request.method == 'POST':
		r = request.get_json()
		user_id = get_current_user()
		request_name = ""
	    
		cursor = db.cursor();
		cursor.execute("SELECT person_id, ifttt_descriptor, ifttt_requests FROM applets")
		result_set = cursor.fetchall()
		for row in result_set:
			if (user_id == row[0]) and (row[1] == r.get('descriptor')):
				request_name = row[2];

		if (request_name == ""):
			return 'Failure'
			
		requests.post(request_name)
		return 'Success'