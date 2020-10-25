/*  Copyright (C) 2014-2020 FastoGT. All right reserved.
    This file is part of fastocloud.
    fastocloud is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    fastocloud is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with fastocloud.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <fastotv/types/input_uri.h>

#define USER_AGENT_FIELD "user_agent"
#define STREAMLINK_URL_FIELD "stream_link"
#define PROXY_FIELD "proxy"
#define PROGRAM_NUMBER_FIELD "program_number"
#define MULTICAST_IFACE_FIELD "multicast_iface"
#define SRT_KEY_FIELD "srt_key"

namespace fastotv {

InputUri::InputUri() : InputUri(0, url_t()) {}

InputUri::InputUri(uri_id_t id, const url_t& input)
    : base_class(id, input), user_agent_(), stream_url_(), http_proxy_url_(), program_number_(), iface_(), srt_key_() {}

bool InputUri::IsValid() const {
  return base_class::IsValid();
}

InputUri::user_agent_t InputUri::GetUserAgent() const {
  return user_agent_;
}

void InputUri::SetUserAgent(user_agent_t agent) {
  user_agent_ = agent;
}

InputUri::stream_url_t InputUri::GetStreamLink() const {
  return stream_url_;
}

void InputUri::SetStreamLink(fastotv::InputUri::stream_url_t stream) {
  stream_url_ = stream;
}

InputUri::http_proxy_url_t InputUri::GetHttpProxyUrl() const {
  return http_proxy_url_;
}

void InputUri::SetHttpProxyUrl(const http_proxy_url_t& url) {
  http_proxy_url_ = url;
}

fastotv::InputUri::program_number_t InputUri::GetProgramNumber() const {
  return program_number_;
}

void InputUri::SetProgramNumber(fastotv::InputUri::program_number_t pid) {
  program_number_ = pid;
}

InputUri::multicast_iface_t InputUri::GetMulticastIface() const {
  return iface_;
}

void InputUri::SetMulticastIface(multicast_iface_t iface) {
  iface_ = iface;
}

InputUri::srt_key_t InputUri::GetSrtKey() const {
  return srt_key_;
}

void InputUri::SetSrtKey(const srt_key_t& pass) {
  srt_key_ = pass;
}

bool InputUri::Equals(const InputUri& url) const {
  return base_class::Equals(url) && url.user_agent_ == user_agent_ && stream_url_ == url.stream_url_ &&
         http_proxy_url_ == url.http_proxy_url_ && program_number_ == url.program_number_ && iface_ == url.iface_ &&
         srt_key_ == url.srt_key_;
}

common::Optional<InputUri> InputUri::Make(common::HashValue* hash) {
  if (!hash) {
    return common::Optional<InputUri>();
  }

  common::Optional<InputUrl> base = base_class::Make(hash);
  if (!base) {
    return common::Optional<InputUri>();
  }

  InputUri url(base->GetID(), base->GetUrl());
  int agent;
  common::Value* agent_field = hash->Find(USER_AGENT_FIELD);
  if (agent_field && agent_field->GetAsInteger(&agent)) {
    url.SetUserAgent(static_cast<UserAgent>(agent));
  }

  common::HashValue* streamlink_url;
  common::Value* streamlink_url_field = hash->Find(STREAMLINK_URL_FIELD);
  if (streamlink_url_field && streamlink_url_field->GetAsHash(&streamlink_url)) {
    url.SetStreamLink(StreamLink::Make(streamlink_url));
  }

  std::string http_url_str;
  common::Value* http_proxy_field = hash->Find(PROXY_FIELD);
  if (http_proxy_field && http_proxy_field->GetAsBasicString(&http_url_str)) {
    url.SetHttpProxyUrl(url_t(http_url_str));
  }

  common::Value* pid_field = hash->Find(PROGRAM_NUMBER_FIELD);
  int pid;
  if (pid_field && pid_field->GetAsInteger(&pid)) {
    url.SetProgramNumber(pid);
  }

  common::Value* iface_field = hash->Find(MULTICAST_IFACE_FIELD);
  std::string iface;
  if (iface_field && iface_field->GetAsBasicString(&iface)) {
    url.SetMulticastIface(iface);
  }

  common::HashValue* srt_key;
  common::Value* srt_key_field = hash->Find(SRT_KEY_FIELD);
  if (srt_key_field && srt_key_field->GetAsHash(&srt_key)) {
    url.SetSrtKey(SrtKey::Make(srt_key));
  }
  return url;
}

common::Error InputUri::DoDeSerialize(json_object* serialized) {
  InputUri res;
  common::Error err = res.base_class::DoDeSerialize(serialized);
  if (err) {
    return err;
  }

  UserAgent agent;
  err = GetEnumField(serialized, USER_AGENT_FIELD, &agent);
  if (!err) {
    res.SetUserAgent(agent);
  }

  json_object* jstream_url = nullptr;
  err = GetObjectField(serialized, STREAMLINK_URL_FIELD, &jstream_url);
  if (!err) {
    StreamLink link;
    err = link.DeSerialize(jstream_url);
    if (!err) {
      res.SetStreamLink(link);
    }
  }

  int pid;
  err = GetIntField(serialized, PROGRAM_NUMBER_FIELD, &pid);
  if (!err) {
    res.SetProgramNumber(pid);
  }

  std::string iface;
  err = GetStringField(serialized, MULTICAST_IFACE_FIELD, &iface);
  if (!err) {
    res.SetMulticastIface(iface);
  }

  json_object* jsrt_key = nullptr;
  err = GetObjectField(serialized, SRT_KEY_FIELD, &jsrt_key);
  if (!err) {
    SrtKey key;
    err = key.DeSerialize(jsrt_key);
    if (!err) {
      res.SetSrtKey(key);
    }
  }

  std::string http_proxy;
  err = GetStringField(serialized, PROXY_FIELD, &http_proxy);
  if (!err) {
    res.SetHttpProxyUrl(url_t(http_proxy));
  }

  *this = res;
  return common::Error();
}

common::Error InputUri::SerializeFields(json_object* deserialized) const {
  if (!IsValid()) {
    return common::make_error_inval();
  }

  common::Error err = base_class::SerializeFields(deserialized);
  if (err) {
    return err;
  }

  if (user_agent_) {
    json_object_object_add(deserialized, USER_AGENT_FIELD, json_object_new_int(*user_agent_));
  }

  if (stream_url_) {
    json_object* jlink = nullptr;
    err = stream_url_->Serialize(&jlink);
    if (!err) {
      json_object_object_add(deserialized, STREAMLINK_URL_FIELD, jlink);
    }
  }

  const auto pid = GetProgramNumber();
  if (pid) {
    json_object_object_add(deserialized, PROGRAM_NUMBER_FIELD, json_object_new_int(*pid));
  }

  const auto iface = GetMulticastIface();
  if (iface) {
    const std::string iface_str = *iface;
    json_object_object_add(deserialized, MULTICAST_IFACE_FIELD, json_object_new_string(iface_str.c_str()));
  }

  if (srt_key_) {
    json_object* jkey = nullptr;
    err = srt_key_->Serialize(&jkey);
    if (!err) {
      json_object_object_add(deserialized, SRT_KEY_FIELD, jkey);
    }
  }

  const auto hurl = GetHttpProxyUrl();
  if (hurl) {
    const std::string proxy = hurl->spec();
    json_object_object_add(deserialized, PROXY_FIELD, json_object_new_string(proxy.c_str()));
  }

  return common::Error();
}

}  // namespace fastotv
