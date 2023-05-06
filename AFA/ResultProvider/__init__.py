import logging
from process import body_to_json

import azure.functions as func


def main(req: func.HttpRequest) -> func.HttpResponse:
	logging.info('Python HTTP trigger function processed a request.')

	#write the code to make sure that the body variable contains the request body as bytes
	body = req.get_body()
	print(body)