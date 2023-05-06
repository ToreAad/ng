import dotenv
dotenv.load_dotenv()

import wave
import struct
import openai
import os
import io

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