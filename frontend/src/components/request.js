import axios from "axios";

const service = axios.create({
  baseURL: "/api",
  timeout: 5000,
});

service.interceptors.request.use(
  (config) => {
    const token = localStorage.getItem("AuthToken");

    if (token) {
      config.headers["Authorization"] = "Bearer " + token;
    }
    return config;
  },
  (error) => {
    console.error("请求配置错误: ", error);
    return Promise.reject(error);
  }
);

service.interceptors.response.use(
  (response) => {
    const res = response.data;
    if (res.code !== 200 && res.code !== 201) {
      console.error("业务错误:", res.message);
      return Promise.reject(new Error(res.message || "Error"));
    } else {
      // 这里已经返回对应的data的 就不用在业务代码里面额外的处理了
      return res;
    }
  },

  (error) => {
    console.error("HTTP 错误: " + error);
    if (error.response && error.response.status === 401) {
    }
    return Promise.reject(error);
  }
);

export default service;
