import wave
import struct
import dotenv
dotenv.load_dotenv()
import openai
import os

openai.api_key = os.environ["OPENAI_API_KEY"]

def convert_raw_to_wav(input_file, output_file, sample_rate=8000, num_channels=1, sample_width=1):
    with open(input_file, 'rb') as f:
        raw_data = f.read()

    num_samples = len(raw_data)

    with wave.open(output_file, 'w') as wav_file:
        wav_file.setnchannels(num_channels)
        wav_file.setsampwidth(sample_width)
        wav_file.setframerate(sample_rate)
        wav_file.setnframes(num_samples)
        
        for sample in raw_data:
            wav_file.writeframes(struct.pack('B', sample))

# convert_raw_to_wav('./data/5.raw', './data/5.wav') 

audio_file = open('./data/5.wav', 'rb')
transcript = openai.Audio.transcribe("whisper-1", audio_file)
print(transcript)
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

print(maybe_json)

from dataclasses import dataclass
from typing import List
from dataclasses_json import dataclass_json
import json

@dataclass_json
@dataclass
class Event:
    name: str
    items: List[str]

try:
    event = Event.from_json(maybe_json)
    print(event)
except json.decoder.JSONDecodeError as e:
    print("Invalid json")
    print(e)