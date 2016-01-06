#!/usr/bin/env python

__author__ = 'mbusel'

import requests as rq
import sys, getopt
import base64
import webbrowser

host = "http://localhost"

def post(login, path):
    with open(path, "rb") as image_file:
        encoded_string = base64.b64encode(image_file.read())
    print encoded_string
    r = rq.post(host + '/post', data = {'login' : login,
                                        'image' : encoded_string
    })
    print r.status_code
    if (r.status_code == 200):
        print 'Successfully posted image!'

def subscribe(login, to):
    r = rq.post(host + "/subscribe", data = {'login' : login,
                                             'to' : to })
    print r.status_code
    if (r.status_code == 200):
        print 'Successfully subscribed ' + login + ' to ' + to

def users():
    r = rq.get(host + '/users')
    print r.status_code
    print r.text

def feed(login, top):
    r = rq.get(host + "/feed?login="+login+"&top="+str(top))
    #print r.status_code
    #print r.text
    for n, item in enumerate(r.json()["items"]):
        with open(item["owner"] + str(n), "wb") as image_file:
            image_file.write(base64.b64decode(item["image"]))
        webbrowser.open(item["owner"] + str(n),2)

def main(argv):
    try:
        opts, args = getopt.getopt(argv, "h", ["mode=", "path=", "login=",
                                               "top=", "to="])
    except getopt.GetoptError:
        print 'main.py mode=....'
        sys.exit(2)

    top = 10
    for opt, arg in opts:
        if opt == '--mode':
            mode = arg
        elif opt == '--path':
            path = arg
        elif opt == '--login':
            login = arg
        elif opt == '--top':
            top = arg
        elif opt == '--to':
            to = arg

    if mode == 'post':
        post(login, path)
    elif mode == 'subscribe':
        subscribe(login, to)
    elif mode == 'users':
        users()
    elif mode == 'feed':
        feed(login, top)


if __name__ == '__main__':
   main(sys.argv[1:])