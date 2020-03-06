#!/usr/bin/python
# -*- coding: UTF-8 -*-

import traceback
import random
import getopt
import sys
import json
import base64

def to_bytes(s):
	if bytes != str:
		if type(s) == str:
			return s.encode('utf-8')
	return s

def to_str(s):
	if bytes != str:
		if type(s) == bytes:
			return s.decode('utf-8')
	return s

def b64decode(data):
	if b':' in data:
		return data
	if len(data) % 4 == 2:
		data += b'=='
	elif len(data) % 4 == 3:
		data += b'='
	return base64.urlsafe_b64decode(data)

def fromlink(link):
	if link[:6] == 'ssr://':
		link = to_bytes(link[6:])
		link = to_str(b64decode(link))
		params_dict = {}
		if '/' in link:
			datas = link.split('/', 1)
			link = datas[0]
			param = datas[1]
			pos = param.find('?')
			if pos >= 0:
				param = param[pos + 1:]
			params = param.split('&')
			for param in params:
				part = param.split('=', 1)
				if len(part) == 2:
					if part[0] in ['obfsparam', 'protoparam']:
						params_dict[part[0]] = to_str(b64decode(to_bytes(part[1])))
					else:
						params_dict[part[0]] = part[1]

		datas = link.split(':')
		if len(datas) == 6:
			host = datas[0]
			port = int(datas[1])
			protocol = datas[2]
			method = datas[3]
			obfs = datas[4]
			passwd = to_str(b64decode(to_bytes(datas[5])))

			result = {}
			result['server'] = host
			result['server_port'] = port
			result['local_address'] = '0.0.0.0'
			result['local_port'] = 1080
			result['password'] = passwd
			result['protocol'] = protocol
			result['method'] = method
			result['obfs'] = obfs
			result['timeout'] = 300
			if 'obfsparam' in params_dict:
				result['obfs_param'] = params_dict['obfsparam']
			if 'protoparam' in params_dict:
				result['protocol_param'] = params_dict['protoparam']
			output = json.dumps(result, sort_keys=True, indent=4, separators=(',', ': '))
			print(output)

def main():
	link = sys.argv[1]
	fromlink(link)

if __name__ == '__main__':
	main()

