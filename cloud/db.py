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


# def addUser(user_id, photo_encoding):
# 	t = Users.create(user_id=user_id, photo_encoding=photo_encoding)
# 	t.save()


def getUser(user_id):
	return Users.select().where(Transcript.user_id == user_id)

def create_tables():
	with db:
		db.create_tables([Users])