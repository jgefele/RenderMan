from __future__ import division
from __future__ import print_function
import hashlib
import os
import platform
import shutil
import sys
import tempfile
import zipfile
if (sys.version_info > (3, 0)):
    from urllib.parse import urlparse
    from urllib.request import urlretrieve
else:
    from urlparse import urlparse
    from urllib import urlretrieve
    


def sha256(filepath):
    hasher = hashlib.sha256()
    with open(filepath, "rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            hasher.update(chunk)
    return hasher.hexdigest()


def makedirs(dir):
    if not os.path.exists(dir):
        os.makedirs(dir)


def unpack(filename, target_dir, subset_to_exract):
    print("Unzipping %s." % filename)
    tmpdir = tempfile.mkdtemp()
    with zipfile.ZipFile(filename) as zip:
        subset = filter(lambda name: subset_to_exract in name, zip.namelist())
        zip.extractall(tmpdir, subset)
    makedirs(target_dir)
    shutil.move(os.path.join(tmpdir, subset_to_exract), target_dir)
    shutil.rmtree(tmpdir)
    print("Done.")


def download(url, size, expected_hash, zip_path):
    print("Downloading %s (%.2f MiB)." % (url, size))
    temp_file_name, _ = urlretrieve(url)
    actual_hash = sha256(temp_file_name)
    if actual_hash != expected_hash:
        os.unlink(temp_file_name)
        sys.exit("File contents unexpected! SHA256 does not match: %s" % actual_hash)
    shutil.move(temp_file_name, zip_path)
    print("Successfully downloaded to %s." % zip_path)


def maybe_download_and_unpack(url, size, expected_hash, download_dir, subset_to_extract):
    url_file = os.path.basename(urlparse(url).path)
    zip_path = os.path.join(download_dir, url_file)
    unpack_dir = os.path.splitext(zip_path)[0]
    plugin_path = os.path.join(unpack_dir, os.path.basename(subset_to_extract))

    if not os.path.exists(plugin_path) and not os.path.exists(zip_path):
        makedirs(download_dir)
        download(url, size, expected_hash, zip_path)

    if not os.path.exists(plugin_path):
        makedirs(unpack_dir)
        unpack(zip_path, unpack_dir, subset_to_extract)

    return plugin_path


def download_plugins():
    if platform.system() == 'Darwin':
        dexed = maybe_download_and_unpack("https://github.com/asb2m10/dexed/releases/download/v0.9.3/dexed-0.9.3-osx.zip",
                                          expected_hash="aad03db9cc7bc2dec14088e92c89cef5f96047faac722332297aa3e389729dd6",
                                          size=5.36, download_dir="plugins", subset_to_extract="dexed-0.9.3-osx/Dexed.vst")
    elif platform.system() == 'Linux':
        dexed = maybe_download_and_unpack("https://github.com/asb2m10/dexed/releases/download/v0.9.3/dexed-0.9.3-lnx.zip",
                                          expected_hash="59a3667f7357f9eb3e032e69d4471eaba4babd686cbd586ac4b4b8807d4f70fc",
                                          size=2.51, download_dir="plugins", subset_to_extract="Dexed.so")
    elif platform.system() == 'Windows':
        dexed = maybe_download_and_unpack("https://github.com/asb2m10/dexed/releases/download/v0.9.3/dexed-0.9.3-win.zip",
                                          expected_hash="b365512bb3eb6ddb2f58db7ae5ca8707fa6ea7029e08cea5165bc8a26a25b38b",
                                          size=2.41, download_dir="plugins", subset_to_extract="dexed-0.9.3/x64/Dexed.dll")
    else:
        raise NotImplementedError()

    return dexed

def make_matplotlib_nbval_testable():
    import matplotlib.pyplot as plt
    import io
    import warnings
    warnings.filterwarnings("ignore")

    def plt_show():
        """The visual effect is more or less the same as plt.show().
           However, this way the 'nbval' notebook tester can check the plots."""
        file_obj = io.BytesIO()
        plt.savefig(file_obj, format='png')
        plt.close()
        display({'image/png':file_obj.getvalue()}, raw=True)

    plt.show = plt_show
