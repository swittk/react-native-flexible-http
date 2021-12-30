import { NativeModules, Platform } from 'react-native';

const LINKING_ERROR =
  `The package 'react-native-flexible-http' doesn't seem to be linked. Make sure: \n\n` +
  Platform.select({ ios: "- You have run 'pod install'\n", default: '' }) +
  '- You rebuilt the app after installing the package\n' +
  '- You are not using Expo managed workflow\n';

const FlexibleHttp = NativeModules.FlexibleHttp
  ? NativeModules.FlexibleHttp
  : new Proxy(
      {},
      {
        get() {
          throw new Error(LINKING_ERROR);
        },
      }
    );

export function multiply(a: number, b: number): Promise<number> {
  return FlexibleHttp.multiply(a, b);
}

export function createHTTPServer() : HTTPLibServer {
  return (global as any).createHTTPServer();
}

// This corresponds to `struct Request` in httplib
export interface HTTPLibServerRequest {
  readonly method: string;
  readonly path: string;
  readonly headers: Record<string, string>;
  readonly body: string;
  readonly remote_addr: string;
  readonly remote_port: number;

  // For server
  readonly version: string;
  readonly target: string;
  readonly params: Record<string, string>;
}

// This corresponds to `struct Response` in httplib
export interface HTTPLibServerResponse {
  readonly version: string;
  status: number;
  reason: string;
  readonly headers: Record<string, string>;
  body: string;
  // Redirect location
  location: string;

  has_header(key: string): boolean;
  set_header(key: string, value: string): void;
  set_redirect(url: string, status?: number): void;
  set_content(content: string, content_type?: string): void;
}


export interface HTTPLibServer {
  listen(port?: number): void;
  stop(): void;
  get(path: string, callback: (request: HTTPLibServerRequest, response: HTTPLibServerResponse)=>void): void;
  post(path: string, callback: (request: HTTPLibServerRequest, response: HTTPLibServerResponse)=>void): void;
  put(path: string, callback: (request: HTTPLibServerRequest, response: HTTPLibServerResponse)=>void): void;
  delete(path: string, callback: (request: HTTPLibServerRequest, response: HTTPLibServerResponse)=>void): void;
}
