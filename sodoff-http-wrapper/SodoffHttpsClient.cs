using System;
using System.Runtime.InteropServices;

namespace sodoff_http_wrapper {
    public class HttpsClient {
        [DllImport("sodoff_http.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr https_get(string url);

        [DllImport("sodoff_http.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void free_string(IntPtr ptr);

        public static string Get(string url) {
            IntPtr ptr = https_get(url);
            if (ptr == IntPtr.Zero)
                return "";

            string result = Marshal.PtrToStringAnsi(ptr);
            free_string(ptr);
            return result;
        }
    }
}
