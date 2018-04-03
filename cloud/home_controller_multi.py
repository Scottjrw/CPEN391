def allowed_file(filename):
    return '.' in filename and \
           filename.rsplit('.', 1)[1].lower() in ALLOWED_EXTENSIONS

def auth_user(user):
	cursor = db.cursor()
	cursor.execute('''DELETE FROM currentUser''')
	currentUser.create(user_id = user.id, username_id = user.username)

def get_current_user():
	cursor = db.cursor()
	cursor.execute("SELECT user_id, username_id FROM currentUser")
	result_set = cursor.fetchall()
	for row in result_set:
		return row[0]


def hex_to_img(hex_string):
	byte_array = bytearray.fromhex(hex_string)
	img = Image.frombytes(img.mode, img.size, byte_array)
	img.save('/home/dchau/img.jpg')
	return 'img conversion done'

def crossdomain(origin=None, methods=None, headers=None,
                max_age=21600, attach_to_all=True,
                automatic_options=True):
    if methods is not None:
        methods = ', '.join(sorted(x.upper() for x in methods))
    if headers is not None and not isinstance(headers, list):
        headers = ', '.join(x.upper() for x in headers)
    if not isinstance(origin, list):
        origin = ', '.join(origin)
    if isinstance(max_age, timedelta):
        max_age = max_age.total_seconds()

    def get_methods():
        if methods is not None:
            return methods

        options_resp = current_app.make_default_options_response()
        return options_resp.headers['allow']

    def decorator(f):
        def wrapped_function(*args, **kwargs):
            if automatic_options and request.method == 'OPTIONS':
                resp = current_app.make_default_options_response()
            else:
                resp = make_response(f(*args, **kwargs))
            if not attach_to_all and request.method != 'OPTIONS':
                return resp

            h = resp.headers

            h['Access-Control-Allow-Origin'] = origin
            h['Access-Control-Allow-Methods'] = get_methods()
            h['Access-Control-Max-Age'] = str(max_age)
            if headers is not None:
                h['Access-Control-Allow-Headers'] = headers
            return resp

        f.provide_automatic_options = False
        return update_wrapper(wrapped_function, f)
    return decorator



@app.route('/getCurrentMapping', methods=['GET'])
def getCurrentMapping():
    if request.method == 'GET':
        user_id = get_current_user()

        cursor = db.cursor()
        user_map = list()

        cursor.execute("SELECT user_id, gesture, ifttt_descriptor FROM Mappings")
        result_set = cursor.fetchall()
        for row in result_set:
            if (user_id == row[0]):
                user_pair = list()
                user_pair.append(row[1])
                user_pair.append(row[2])
                user_map.append(user_pair)

        return jsonify(user_map)


@app.route('/getCurrentUser', methods=['GET'])
def getCurrentUser():
    if request.method == "GET":
        cursor = db.cursor()
        cursor.execute("SELECT user_id, username_id FROM currentUser")
        result_set = cursor.fetchall()
        for row in result_set:
            return row[1]

@app.route('/logout', methods=['GET'])
def logout():
    cursor = db.cursor()
    cursor.execute('''DELETE FROM currentUser''')
    return 'Success'