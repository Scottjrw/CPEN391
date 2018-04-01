from peewee import *
from playhouse.sqlite_ext import SqliteExtDatabase
import datetime

db = SqliteExtDatabase('database.db')


class BaseModel(Model):
	class Meta:
		database = db


class Users(BaseModel):
	username = CharField(unique=True)
	password = CharField()
	photo_encoding = BlobField()

class Applets(BaseModel):
	person_id = IntegerField()
	ifttt_descriptor = CharField()
	ifttt_requests = CharField()

class tempPicture(BaseModel):
	picture_segment = CharField()

class currentUser(BaseModel):
	user_id = IntegerField()
	username_id = IntegerField()
	logged_in = BooleanField()


# def addUser(user_id, photo_encoding):
# 	t = Users.create(user_id=user_id, photo_encoding=photo_encoding)
# 	t.save()

def create_tables():
	with db:
		db.create_tables([Users, Applets, tempPicture, currentUser])