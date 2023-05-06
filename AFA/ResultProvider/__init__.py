import logging

import azure.functions as func

import wave
import struct
import openai
import os
import io
import json
import requests

openai.api_key = os.environ["OPENAI_API_KEY"]

def convert_raw_to_wav(raw_data, sample_rate=8000, num_channels=1, sample_width=1):
	# raw_data = input_file.read()

	num_samples = len(raw_data)

	# Create binary in memory file-like object called wav_file using io.BytesIO
	wav_file_io = io.BytesIO()
	wav_file_io.name = 'audio.wav'
	wav_file = wave.open(wav_file_io, 'w')
	wav_file.setnchannels(num_channels)
	wav_file.setsampwidth(sample_width)
	wav_file.setframerate(sample_rate)
	wav_file.setnframes(num_samples)
	
	for sample in raw_data:
		wav_file.writeframes(struct.pack('B', sample))
	wav_file_io.seek(0)
	return wav_file_io

def transcribe_audio(audio_file):
	transcript = openai.Audio.transcribe("whisper-1", audio_file)
	return transcript

def get_json_from_transcript(transcript):
	prompt_template = """From the following extract the name and the items thrown. Structure it as a json like:
	{{"name":"some_name", "items":["Some_items1", "Some_items2", "Some_items3"]}}
	To extract:
	{transcript}
	"""

	maybe_json = openai.Completion.create(
	model="text-davinci-003",
	prompt=prompt_template.format(transcript=transcript),
	max_tokens=256,
	temperature=0
	).choices[0].text

	return maybe_json

def body_to_json(body):
	audio_file = convert_raw_to_wav(body)
	transcript = transcribe_audio(audio_file)
	return get_json_from_transcript(transcript)

import http.client
import json
def post_new_body(new_body):
    body = json.dumps(new_body)

    headers = {'Content-Type': 'application/json'}

    # Parsing the URL
    url = "https://prod-25.norwayeast.logic.azure.com/workflows/7fe84fbbe3104e0daa66a8c82689dfdc/triggers/manual/paths/invoke?api-version=2016-10-01&sp=%2Ftriggers%2Fmanual%2Frun&sv=1.0&sig=CbWeA6_jcbOEAzck45AjtsoQDI_r6PZWb4VuFESvy4U"
    parsed_url = http.client.urlsplit(url)

    # Creating an HTTPS connection
    conn = http.client.HTTPSConnection(parsed_url.hostname)

    # Sending a POST request
    conn.request("POST", parsed_url.path + "?" + parsed_url.query, body=body, headers=headers)

    # Receiving the response
    res = conn.getresponse()

    print(res.status, res.reason)
    print(res.read().decode())

    # Closing the connection
    conn.close()

def main(req: func.HttpRequest) -> func.HttpResponse:
	logging.info('Python HTTP trigger function processed a request.')

	#write the code to make sure that the body variable contains the request body as bytes
	body = req.get_body()
	output = body_to_json(body)
	try:
			res = json.loads(output)
			name = res["name"]
			for item in res["items"]:
					new_body = {"name": name, "item": item}
					# post the new body to "https://prod-25.norwayeast.logic.azure.com/workflows/7fe84fbbe3104e0daa66a8c82689dfdc/triggers/manual/paths/invoke?api-version=2016-10-01&sp=%2Ftriggers%2Fmanual%2Frun&sv=1.0&sig=CbWeA6_jcbOEAzck45AjtsoQDI_r6PZWb4VuFESvy4U"
					# using the requests library
					# requests.post("https://prod-25.norwayeast.logic.azure.com/workflows/7fe84fbbe3104e0daa66a8c82689dfdc/triggers/manual/paths/invoke?api-version=2016-10-01&sp=%2Ftriggers%2Fmanual%2Frun&sv=1.0&sig=CbWeA6_jcbOEAzck45AjtsoQDI_r6PZWb4VuFESvy4U", json=new_body)
					post_new_body(new_body)
	except Exception as e:
			return func.HttpResponse( body="Error: " + str(e), status_code=400 )
	return func.HttpResponse( body=body, status_code=200 )