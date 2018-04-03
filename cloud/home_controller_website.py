@app.route('/joinByRGB', methods=['GET', 'POST'])
def joinByRGB():
	if request.method == 'POST':

		width = int(request.form['width'])
		height = int(request.form['height'])

		list_of_pixels = list()
		rgb_string = request.form['picture']
		rgb_list = rgb_string.split(",")

		for i in range(0, len(rgb_list)):
			if (i < len(rgb_list) - 4):
				if (i % 4 == 0):
					r_value = int(rgb_list[i])
					g_value = int(rgb_list[i+1])
					b_value = int(rgb_list[i+2])
					rgb_tuple = (r_value,g_value,b_value)
					list_of_pixels.append(rgb_tuple)


		im = Image.new('RGB', (width,height))
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

		if (request.form['username'] == ""):
			print('no username')
			return 'User failed to join'

		if (request.form['password'] == ""):
			print('no password')
			return 'User failed to join'


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
			return request.form['username']

		except IntegrityError:
			print('That username is already taken')
			return 'Username Taken'
		return 'Fail'




@app.route('/loginByRGB', methods=['GET', 'POST'])    
def loginByRGB():
	if request.method == 'POST':
		width = int(request.form['width'])
		height = int(request.form['height'])

		list_of_pixels = list()
		rgb_string = request.form['picture']
		rgb_list = rgb_string.split(",")

		for i in range(0, len(rgb_list)):
			if (i < len(rgb_list) - 4):
				if (i % 4 == 0):
					r_value = int(rgb_list[i])
					g_value = int(rgb_list[i+1])
					b_value = int(rgb_list[i+2])
					rgb_tuple = (r_value,g_value,b_value)
					list_of_pixels.append(rgb_tuple)

		im = Image.new('RGB', (width,height))
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
				return str(row[0])

		return 'Fail'



@app.route('/loginByPasswordWebsite', methods=['GET', 'POST', 'OPTIONS'])    
def loginByPasswordWebsite():
	if request.method == 'POST' or request.method == 'OPTIONS':

		username=request.form['username']

		password=md5((request.form['password']).encode('utf-8')).hexdigest()

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





@app.route('/changeCurrentMapping', methods=['POST', 'OPTIONS'])
def changeCurrentMapping():
	if request.method == 'POST' or request.method == 'OPTIONS':
		user_id = get_current_user()

		cursor = db.cursor()
		cursor.execute('''DELETE FROM Mappings WHERE user_id=?''', (user_id,))

		mapping_list = request.form['mapping'].split(",")

		for i in range(0, len(mapping_list)):
			if (i <= len(mapping_list) - 2):
				if (i % 2 == 0):
					gesture = mapping_list[i]
					action = mapping_list[i+1]
					Mappings.create(user_id = user_id, gesture = gesture, ifttt_descriptor = action)

		return 'Success'




@app.route('/addApplet', methods=['POST'])
def addApplet():
	if request.method == 'POST':

		user_id = get_current_user()

		cursor = db.cursor()

		cursor.execute("SELECT person_id, ifttt_descriptor FROM applets")
		result_set = cursor.fetchall()
		for row in result_set:
			if (user_id == row[0]) and (request.form['descriptor'] == row[1]):
				return 'Service Exists'

		Applets.create(person_id = user_id, ifttt_descriptor = request.form['descriptor'], ifttt_requests = request.form['url'])

		return 'Success'




@app.route('/deleteApplet', methods=['POST'])
def deleteApplet():
	if request.method == 'POST':

		user_id = get_current_user()
		

		cursor = db.cursor()


		cursor.execute('''DELETE FROM applets WHERE person_id AND ifttt_requests=?''', (user_id, request.form['descriptor']))

		return 'Removed applet'





@app.route('/getUserApplets', methods=['GET'])
def getUserApplets():
	if request.method == 'GET':
		user_id = get_current_user()

		cursor = db.cursor()
		list_of_applets = list()

		cursor.execute("SELECT person_id, ifttt_descriptor FROM applets")
		result_set = cursor.fetchall()
		for row in result_set:
			if (user_id == row[0]):
				list_of_applets.append(row[1])

		print(list_of_applets)

		return jsonify(list_of_applets)