using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Unosquare.Labs.EmbedIO;
using Unosquare.Labs.EmbedIO.Constants;
using Unosquare.Labs.EmbedIO.Modules;
using Unosquare.Net;

namespace VVVV.Nodes.LinesDatabase
{
    class APIController : WebApiController
    {
        [WebApiHandler(HttpVerbs.Get, "/api/lines")]
        public bool GetLines(WebServer server, HttpListenerContext context, int id)
        {
            try
            {
                var data = LinesData.Instance;

                //update ages
                var now = DateTime.Now;
                foreach (var line in data.Lines)
                {
                    line.Age = now.Subtract(line.LastUpdate).TotalSeconds;
                }

                var successResponse = new
                {
                    success = true,
                    content = data.Lines
                };
                return context.JsonResponse(JsonConvert.SerializeObject(successResponse));
            }
            catch (Exception e)
            {
                return HandleError(context, e, (int)System.Net.HttpStatusCode.InternalServerError);
            }
        }

        [WebApiHandler(HttpVerbs.Get, "/api/newline")]
        public bool AddLine(WebServer server, HttpListenerContext context, int id)
        {
            try
            {
                var data = LinesData.Instance;
                var newLine = data.NewLine();
                data.Lines.Add(newLine);
                var successResponse = new
                {
                    success = true,
                    content = newLine
                };
                return context.JsonResponse(JsonConvert.SerializeObject(successResponse));
            }
            catch (Exception e)
            {
                return HandleError(context, e, (int)System.Net.HttpStatusCode.InternalServerError);
            }
        }

        [WebApiHandler(HttpVerbs.Post, "/api/newline")]
        public bool AddLinePost(WebServer server, HttpListenerContext context, int id)
        {
            try
            {
                var data = LinesData.Instance;
                var requestBody = context.RequestBody();
                var newLine = JsonConvert.DeserializeObject<Line>(requestBody);
                data.AddLine(newLine);
                var successResponse = new
                {
                    success = true,
                    content = newLine
                };
                return context.JsonResponse(JsonConvert.SerializeObject(successResponse));
            }
            catch (Exception e)
            {
                return HandleError(context, e, (int)System.Net.HttpStatusCode.InternalServerError);
            }
        }

        [WebApiHandler(HttpVerbs.Post, "/api/updateline")]
        public bool UpdateLine(WebServer server, HttpListenerContext context, int id)
        {
            try
            {
                var data = LinesData.Instance;
                var lineData = JsonConvert.DeserializeObject<Line>(context.RequestBody());
                foreach(var line in data.Lines)
                {
                    if(line.LineIndex == lineData.LineIndex)
                    {
                        line.Start = lineData.Start;
                        line.End = lineData.End;
                        line.LastUpdate = DateTime.Now;
                        line.LastEditBy = lineData.LastEditBy;
                    }
                }
                //check what is in lineData using a breakpoint!

                var successResponse = new
                {
                    success = true,
                };
                return context.JsonResponse(JsonConvert.SerializeObject(successResponse));
            }
            catch (Exception e)
            {
                return HandleError(context, e, (int)System.Net.HttpStatusCode.InternalServerError);
            }
        }

        [WebApiHandler(HttpVerbs.Post, "/api/insertlines")]
        public bool InsertLines(WebServer server, HttpListenerContext context, int id)
        {
            try
            {
                var data = LinesData.Instance;
                var requestBody = context.RequestBody();
                var newLines = JsonConvert.DeserializeObject<List<Line>>(requestBody);
                foreach (var newLine in newLines)
                {
                    data.AddLine(newLine);
                }
                var successResponse = new
                {
                    success = true,
                    content = newLines
                };
                return context.JsonResponse(JsonConvert.SerializeObject(successResponse));
            }
            catch (Exception e)
            {
                return HandleError(context, e, (int)System.Net.HttpStatusCode.InternalServerError);
            }
        }

        [WebApiHandler(HttpVerbs.Post, "/api/deleteline")]
        public bool DeleteLine(WebServer server, HttpListenerContext context, int id)
        {
            try
            {
                var data = LinesData.Instance;
                var requestBody = context.RequestBody();
                var requestObject = JsonConvert.DeserializeObject<Line>(requestBody);
                data.Lines.RemoveAll(line => line.LineIndex == requestObject.LineIndex);
                var successResponse = new
                {
                    success = true
                };
                return context.JsonResponse(JsonConvert.SerializeObject(successResponse));
            }
            catch (Exception e)
            {
                return HandleError(context, e, (int)System.Net.HttpStatusCode.InternalServerError);
            }
        }

        [WebApiHandler(HttpVerbs.Post, "/api/pingline")]
        public bool PingLine(WebServer server, HttpListenerContext context, int id)
        {
            try
            {
                var data = LinesData.Instance;
                var requestBody = context.RequestBody();
                var requestObject = JsonConvert.DeserializeObject<Line>(requestBody);
                foreach(var line in data.Lines)
                {
                    if(line.LineIndex == requestObject.LineIndex)
                    {
                        line.LastUpdate = DateTime.Now;
                    }
                }
                var successResponse = new
                {
                    success = true
                };
                return context.JsonResponse(JsonConvert.SerializeObject(successResponse));
            }
            catch (Exception e)
            {
                return HandleError(context, e, (int)System.Net.HttpStatusCode.InternalServerError);
            }
        }

        protected bool HandleError(HttpListenerContext context, Exception e, int statusCode = 500)
        {
            var errorResponse = new
            {
                success = false,
                error = new
                {
                    Title = "Unexpected error",
                    ErorrCode = e.GetType().Name,
                    Description = e.Message
                }
            };
            context.Response.StatusCode = statusCode;
            return context.JsonResponse(errorResponse);
        }
    }
}
