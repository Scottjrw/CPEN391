from peewee import *
from playhouse.sqlite_ext import SqliteExtDatabase
import datetime

db = SqliteExtDatabase('database.db')


class BaseModel(Model):
    class Meta:
        database = db


class Users(BaseModel):
    user_id = TextField()
    photo_encoding = TextField()
    #created_date = DateTimeField(default=datetime.datetime.now)


def addUser(user_id, photo_encoding):
    t = Users.create(user_id=user_id, photo_encoding=photo_encoding)
    t.save()


def getUser(user_id):
    return Users.select().where(Transcript.user_id == user_id)