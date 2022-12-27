#!/usr/bin/python3

import requests
import base64
import json

import sys

def main():
    args = sys.argv
    if len(args) != 3:
        print(f'usage: {args[0]} <book_id> <filename>')
        return

    book_id = args[1]
    filename = args[2]

    cookies = {}
    try:
        with open('cookies.json', 'r') as f:
            cookies = json.loads(f.read())
    except Exception as e:
        print(e)

    url = f'https://reader.lanbook.com/api/v2/book/{book_id}/documentFile?base64=1&lms='

    print(f'fetching book {book_id}...')
    response = requests.get(url, cookies=cookies)
    if response.status_code != 200:
        print(f'ERROR: status code: {response.status_code}')
        return

    print(f'writing to "{filename}"...')
    data = response.json()['data']
    with open(filename, 'wb') as f:
        f.write(base64.b64decode(data))

    print('done')

if __name__ == '__main__':
    main()

