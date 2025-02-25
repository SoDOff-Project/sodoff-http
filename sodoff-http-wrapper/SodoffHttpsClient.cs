using System;
using System.Runtime.InteropServices;

namespace sodoff_http_wrapper {
    public class HttpsClient {
        [DllImport("sodoff_http.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr https_get(string url, out uint length);

        [DllImport("sodoff_http.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr https_post(string url, string form, out uint length);

        [DllImport("sodoff_http.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void free_buffer(IntPtr ptr);

        public static byte[] Get(string url) {
            IntPtr ptr = https_get(url, out uint length);
            if (ptr == IntPtr.Zero)
                return new byte[0];

            byte[] result = new byte[length];
            Marshal.Copy(ptr, result, 0, (int)length);
            free_buffer(ptr);
            return result;
        }

        public static byte[] Post(string url, string form) {
            IntPtr ptr = https_post(url, form, out uint length);
            if (ptr == IntPtr.Zero)
                return new byte[0];

            byte[] result = new byte[length];
            Marshal.Copy(ptr, result, 0, (int)length);
            free_buffer(ptr);
            return result;
        }
    }
}
