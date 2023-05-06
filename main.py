from process import body_to_json

raw_data = open('./data/5.raw', 'rb').read()
print(body_to_json(raw_data))
