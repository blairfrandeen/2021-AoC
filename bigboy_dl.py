# Back to python oh no.
from urllib.request import urlopen
from pathlib import Path
import re
import subprocess
import time

# website containing big data files
bigdata_url = "https://the-tk.com/project/aoc2021-bigboys.html"

# regex to find any urls that are linked to zipped (.xz) files
url_regex = re.compile("(?<=<a href=\")https:\/\/[\w./-]+.xz(?=\">)")

# Regex to pull the file handle out of a URL
filename_regex = re.compile("\d+-[\w-]+(?=.in.xz)")

# where to put this data
bigdata_folder = "~/advent/bigdata"

# open the webpage and store the html source in a string
page = urlopen(bigdata_url)
html_bytes = page.read()
html_source = html_bytes.decode("utf-8")

# find all URLs in the page that we might want to download
url_list = re.findall(url_regex, html_source)

for url in url_list:
    # Apply regexes
    file_handle = re.search(filename_regex, url)[0]
    data_file_path = bigdata_folder + '/' + file_handle
    zip_file_path = data_file_path + '.xz'

    # dummy command in case nothing to unzip
    unzip_command = ''

    download = False # by default we don't download anything
    # check if the zipped file or the unzipped file already exists
    # if so, print that to the terminal instead
    if (Path(data_file_path).expanduser().is_file()):
        download_command = f'echo File {data_file_path} already exists.'
    elif (Path(zip_file_path).expanduser().is_file()):
        download_command = f'echo File {zip_file_path} already exists.'
    else:
        download_command = f'curl {url} --create-dirs -o {zip_file_path}'
        unzip_command = f'xz -d {zip_file_path}'
        download = True

    # download the file (or state that we have it)
    dl_process = subprocess.Popen(download_command, shell=True,
            stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    dl_out = dl_process.communicate()[0]
    print(str(dl_out, "utf-8"), end="")

    if download:
        try: # make sure we have the file to unzip
            Path(zip_file_path).expanduser().is_file()
        except: #if not, wait until it downloads
            time.sleep(2)

        process=subprocess.Popen(unzip_command, shell=True)


