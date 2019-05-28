#coding:utf-8
import requests
import json

import random

import socket
sk = socket.socket()# 我买一个新手机
sk.bind(('127.0.0.1',9090))# 买一个电话卡，装上
sk.listen(5)# 告诉了几个人我的手机号
fd,addr = sk.accept()# 等着别人给打电话


tag = 'japan'
url = 'https://www.instagram.com/explore/tags/' + tag + '/?__a=1'

# keys = [
# 	'id', 'owner', 'display_url', 'shortcode', 'edge_liked_by'
# 	]
keys = [
	 'display_url'
	]
keys_caption = ['text']


def jprint(data_dict):
	print(json.dumps(data_dict, indent = 4))

# 1 - Grab the data

def get_ig_page(url, session = None):
	#print(url)
	session = session or requests.Session()

	r = session.get(url)
	r_code = r.status_code
	#print(r_code)

	if r.status_code == requests.codes.ok:
		#the code is 200 or valid
		return r
	else:
		return None

def flatten_list(a_list):
	if a_list:
		flat = ' '.join(list_item for list_item in a_list if list_item is not None)
		return flat
	return None

def get_tag(text):
	tag = set()
	if text is not None:
		text = text.split(' ')
		#jprint(text)
		for word in text:
			if word.startswith('#'):
				word = word.strip('#')
				yield word

captions = list()

photolist = list()

ig_data_dict = get_ig_page(url)

if ig_data_dict is not None:
	ig_data_dict = ig_data_dict.json()
	#jprint(ig_data_dict)

	data1 = ig_data_dict.get('graphql', None)
	data = data1.get('hashtag', None)

     # getting top posts
	top_posts = data.get('edge_hashtag_to_top_posts', None)
	if top_posts is not None:
		top_posts = top_posts.get('edges')
	 	

    # getting recent posts
	recent_posts = data.get('edge_hashtag_to_media', None)
	if recent_posts is not None:
		recent_posts = recent_posts.get('edges')

	posts = top_posts + recent_posts
	# //print the key's content
	# for post in posts:
	# 	post_use = post.get('node')
	# 	#jprint(post_use)
	# 	for key in keys:
	# 		print(key.upper(), ':', post_use.get(key))

	# //print the photo url
	for post in posts:
		post_use = post.get('node')
		post_display = post_use.get('display_url', None)
		if post_display is not None:
			photolist.append(post_display)

	# x = 12
	# jprint(photolist[x])	

	

	for post in posts:
		post_use = post.get('node')
		post_caption = post_use.get('edge_media_to_caption', None)
		if post_caption is not None:
			post_caption = post_caption.get('edges')

			for captions_temp in post_caption:
				captions_temp_use = captions_temp.get('node')
				captions_temp_use = captions_temp_use.get('text', None)
				if captions_temp_use is not None:
					captions.append(captions_temp_use)

	#jprint(captions[3])
	# print('We have', len(captions), 'captions')

	#flat_captions = flatten_list(captions)

	#if flat_captions is not None:
		#tags = list(get_tag(flat_captions))

		# print('We have', len(tags), 'tags in totoal')
		#for tag in tags:
			# jprint('#{}'.format(tag))

		# for post_temp in post_use:
		# 	for key in keys:
		# 		print(key.upper(), ':', post_temp.get(key))
	print('photolist length:', len(photolist))
	photoListString = str(len(photolist))
	sendLength = 'IDP' + photoListString

else:
	print('Oops something went wrong!')


while 1:
	msg_r = fd.recv(1024).decode('utf-8')
	if msg_r == 'q':
		break
	print(msg_r)

	if msg_r == 's':
		fd.send(sendLength.encode('utf-8'))
    #msg_s = input("Please input your messages:")
    #msg_s = input("Please input the number:")


	elif msg_r == 'o':
		photolist = []
		j = random.randint(0,18)
		if j == 0:
			tag = 'kumamon'
		elif j == 1:
			tag = 'hellokitty'
		elif j == 2:
			tag = 'tokusatsu'
		elif j == 3:
			tag = 'tokyolove'
		elif j == 4:
			tag = 'finalfantasy'
		elif j == 5:
			tag = 'tokyojungle'
		elif j == 6:
			tag = 'manga'
		elif j == 7:
			tag = 'purikura'
		elif j == 8:
			tag = 'ghibi'
		elif j == 9:
			tag = 'miku'
		elif j == 10:
			tag = 'hanabi'
		elif j == 11:
			tag = 'kamishibai'
		elif j == 12:
			tag = 'shibuya'
		elif j == 13:
			tag = 'akihabara'
		elif j == 14:
			tag = 'kawaii'
		elif j == 15:
			tag = 'akb'
		elif j == 16:
			tag = 'superflat'															
		else:
			tag = 'japan'

		url = 'https://www.instagram.com/explore/tags/' + tag + '/?__a=1'
		print(url)
		ig_data_dict = get_ig_page(url)
		if ig_data_dict is not None:
			ig_data_dict = ig_data_dict.json()
			#jprint(ig_data_dict)

			data1 = ig_data_dict.get('graphql', None)
			data = data1.get('hashtag', None)

		     # getting top posts
			top_posts = data.get('edge_hashtag_to_top_posts', None)
			if top_posts is not None:
				top_posts = top_posts.get('edges')
			 	
		    # getting recent posts
			recent_posts = data.get('edge_hashtag_to_media', None)
			if recent_posts is not None:
				recent_posts = recent_posts.get('edges')

			posts = top_posts + recent_posts

			for post in posts:
				post_use = post.get('node')
				post_display = post_use.get('display_url', None)
				if post_display is not None:
					photolist.append(post_display)

			photoListString = str(len(photolist))
			sendLength = 'IDP' + photoListString

		else:
			print('Oops something went wrong!')



	else:
		msg_s = msg_r
		str_temp = photolist[int(msg_s)]
		fd.send(str_temp.encode('utf-8'))
		if msg_s == 'q':
			break

		


fd.close()# 挂断电话
sk.close()# 关机

