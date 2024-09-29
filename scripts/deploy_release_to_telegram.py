#!/usr/bin/env python3
"""
Deploy release binaries to telegram
"""
import os
import logging
import argparse
import subprocess
import requests
from typing import Optional

logger = logging.getLogger(__name__)

def get_git_info():
    commit_sha = subprocess.check_output(['git', 'rev-parse', '--short=8', 'HEAD']).decode('utf-8').strip()
    commit_date = subprocess.check_output(['git', 'log', '-1', '--format=%cd', '--date=short']).decode('utf-8').strip()
    committer_name = subprocess.check_output(['git', 'log', '-1', '--format=%an']).decode('utf-8').strip()
    committer_email = subprocess.check_output(['git', 'log', '-1', '--format=%ae']).decode('utf-8').strip()
    return commit_sha, commit_date, committer_name, committer_email

def find_files(dir) -> Optional[list]:
    files = sorted([f for f in os.listdir(dir) if f.endswith('.bin')])
    if len(files) < 5:
        print("Error: Need at least 5 .bin files to send.")
        return

    files = [os.path.join('build/release', file) for file in files]

    return files

def create_text_message() -> str:
    commit_sha, commit_date, committer_name, committer_email = get_git_info()
    return (
        "Mini node\n"
        f"VCS commit: {commit_sha}\n"
        f"Commit date: {commit_date}\n"
        f"Author: {committer_name} <{committer_email}>"
    )

def send_media_group(telegram_bot_token, telegram_chat_id, files, caption):
    media_json_array = [
        {"type": "document", "media": "attach://file1"},
        {"type": "document", "media": "attach://file2"},
        {"type": "document", "media": "attach://file3"},
        {"type": "document", "media": "attach://file4"},
        {"type": "document", "media": "attach://file5", "caption": caption},
    ]

    media_payload = {
        'chat_id': telegram_chat_id,
        'media': str(media_json_array).replace("'", '"')
    }

    files_payload = {
        'file1': open(files[0], 'rb'),
        'file2': open(files[1], 'rb'),
        'file3': open(files[2], 'rb'),
        'file4': open(files[3], 'rb'),
        'file5': open(files[4], 'rb'),
    }

    response = requests.post(f'https://api.telegram.org/bot{telegram_bot_token}/sendMediaGroup', data=media_payload, files=files_payload)

    for file in files_payload.values():
        file.close()

    return response.json()

def main():
    parser = argparse.ArgumentParser(description="Send media group via Telegram bot.")
    parser.add_argument('--bot-token', required=True, help='Telegram bot token')
    parser.add_argument('--chat-id', required=True, help='Telegram chat ID')
    parser.add_argument('--input-dir', default='build/release')
    args = parser.parse_args()

    files = find_files(dir=args.input_dir)
    if files is None:
        os.exit(1)

    text_message = create_text_message()

    send_media_group(args.bot_token, args.chat_id, files, text_message)

if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO)
    logger.setLevel(logging.INFO)

    main()
