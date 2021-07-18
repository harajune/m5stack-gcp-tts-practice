from google.cloud import texttospeech, secretmanager
from flask import make_response
from flask_httpauth import HTTPBasicAuth

auth = HTTPBasicAuth()

def basic_password():
    client = secretmanager.SecretManagerServiceClient()
    name = f"projects/976862718067/secrets/tts-secret/versions/latest"
    secretmanager_response = client.access_secret_version(name=name)
    return secretmanager_response.payload.data.decode('UTF-8')

id_list = {
    "paparobot": basic_password()
}

@auth.get_password
def get_pw(id):
    return id_list.get(id)

@auth.login_required
def tts(request):
    """Responds to any HTTP request.
    Args:
        request (flask.Request): HTTP request object.
    Returns:
        The response text or any set of values that can be turned into a
        Response object using
        `make_response <http://flask.pocoo.org/docs/1.0/api/#flask.Flask.make_response>`.
    """
    text = ''
    request_json = request.get_json()
    if request.args and 'text' in request.args:
        text = request.args.get('text')
    else:
        return 'error'

    # Instantiates a client
    client = texttospeech.TextToSpeechClient()

    # Set the text input to be synthesized
    synthesis_input = texttospeech.SynthesisInput(text=text)

    voice = texttospeech.VoiceSelectionParams(
        language_code="ja-JP" #, ssml_gender=texttospeech.SsmlVoiceGender.NEUTRAL
    )

    # Select the type of audio file you want returned
    audio_config = texttospeech.AudioConfig(
        audio_encoding=texttospeech.AudioEncoding.MP3
    )

    # Perform the text-to-speech request on the text input with the selected
    # voice parameters and audio file type
    tts_response = client.synthesize_speech(
        input=synthesis_input, voice=voice, audio_config=audio_config
    )

    # The response's audio_content is binary.
    http_response = make_response(tts_response.audio_content)
    http_response.headers.set("Content-Type", "audio/mpeg")

    return http_response
