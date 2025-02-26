using System.Text;
using System.Threading;

namespace sodoff_http_wrapper {
    public class WWWWrapper {
        private byte[] mData;
        private bool mDone = false;
        private string mError;
        private string mUrl;

        public WWWWrapper(string url) {
            mUrl = url;
            Thread thd = new Thread(() => BackgroundGet(url));
            thd.Start();
        }

        public WWWWrapper(string url, string formString) {
            mUrl = url;
            Thread thd = new Thread(() => BackgroundPost(url, formString));
            thd.Start();
        }

        private void BackgroundGet(string url) {
            mData = HttpsClient.Get(url);
            mDone = true;
        }

        private void BackgroundPost(string url, string formString) {
            mData = HttpsClient.Post(url, formString);
            if (url.Contains("CheckProductVersion")) {
                mError = "404"; // TODO: This is a workaround. The native library doesn't pass errors yet.
            }
            mDone = true;
        }

        public string text {
            get {
                return Encoding.Default.GetString(mData);
            }
        }

        public byte[] bytes {
            get {
                return mData;
            }
        }

        public int size {
            get {
                return mData.Length;
            }
        }

        public string error {
            get {
                return mError;
            }
        }

        public bool done {
            get {
                return mDone;
            }
        }
        
        public string url {
            get {
                return mUrl;
            }
        }
    }
}
