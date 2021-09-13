import requests
from typing import Optional
import os
import time


def upload_image(vs3_host, jpeg_image_bytes, image_name: str) -> Optional[str]:
    content_type = "image/jpeg"
    bucket_name = "deeperlook3"
    key_name = "dd-aa"
    api_url = f"{vs3_host}"
    ret: Optional[str] = None
    try:
        # LOGGER.info(f"Trying to upload in vs3 url {api_url}")
        files = {"file": (image_name, jpeg_image_bytes, content_type)}
        payload = {
            "Bucket": bucket_name,
            "Key": key_name,
            "ContentType": content_type,
        }
        r = requests.request(method="POST",
                             url=api_url,
                             data=payload,
                             files=files)
        # LOGGER.info(f"upload_image, status: {r.status_code}, text: {r.text}")

        if r.status_code == 201 or r.status_code == 200:
            json_response = r.json()
            ret = str(json_response["items"][0]["Location"])
    except requests.exceptions.ConnectionError as e:
        print(f"upload_image connection error {e}")
    return ret


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser("HTTP File server uploader")
    parser.add_argument("vs3_host", type=str, help='vs3 host')
    parser.add_argument("vs3_port", type=str, help='vs3 port')
    parser.add_argument("filepath_to_upload",
                        type=str,
                        help="file upload path, preferably absolute path")
    parser.add_argument("--number_of_times_to_upload",
                        type=int,
                        default=10,
                        help="Number of times to upload the file in loop")
    parser.add_argument(
        "--tts",
        type=int,
        default=30,
        help="Time to sleep in miliseconds between each iteration")
    parser.add_argument("--profile",
                        action='store_true',
                        help="Profiling in between each call")
    args = parser.parse_args()
    vs3_host = args.vs3_host
    vs3_port = args.vs3_port
    vs3_upload_url = f"http://{vs3_host}:{vs3_port}/vs3"
    filename = args.filepath_to_upload.split(os.sep)[-1]
    bytes_to_send = None
    my_pid = os.getpid()
    for i in range(0, args.number_of_times_to_upload):
        try:
            with open(args.filepath_to_upload, 'rb') as f:
                bytes_to_send = f.read()
            if args.profile:
                et = (time.time()) * 1000.0
            vs3_upload_url = f"http://{vs3_host}:{vs3_port}/vs3/{my_pid:010d}/{i:010d}"
            ret = upload_image(vs3_upload_url, bytes_to_send, filename)
            if ret is not None:
                if args.profile:
                    st = (time.time()) * 1000.0
                    print(f"Iteration {i} Time taken to upload {st-et} ms")
                    #print(f"Upload path {ret}")
            else:
                print("upload failed")
            time.sleep(args.tts / 1000.0)

        except FileNotFoundError as e:
            print(f"Please provide proper file path {e}")
            break
        except KeyboardInterrupt as e:
            print("Closing due to Keyboard Interupt")
            break
    print("Closing VS3 uploader")
