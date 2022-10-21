package ru.itmo.wp.servlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.net.URLDecoder;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;

public class StaticServlet extends HttpServlet {
    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws IOException {
        String[] uris = request.getRequestURI().split("\\+");
        String path = URLDecoder.decode(getServletContext().getRealPath("."), StandardCharsets.UTF_8);
        if (!path.endsWith("src\\main\\webapp\\static")) {
            if (path.endsWith("src\\main\\webapp")) {
                path += "\\static";
            } else {
                path += "/../../src/main/webapp/static";
            }
        }
        boolean isContentDeterment = false;

        for (String uri : uris) {
            if (!uri.startsWith("/")) {
                uri = "/" + uri;
            }

            if (!Files.exists(Paths.get(path, uri))) {
                if (!Files.exists(Paths.get(getServletContext().getRealPath("/static" + uri)))) {
                    response.sendError(HttpServletResponse.SC_NOT_FOUND);
                    return;
                }
            }

        }

        for (String uri : uris) {

            File file = new File(path, uri);

            if (!file.isFile()) {
                file = new File(getServletContext().getRealPath("/static" + uri));
            }

            if (!isContentDeterment) {
                setContType(uri, response);
                isContentDeterment = true;
            }
            try (OutputStream outputStream = response.getOutputStream()) {
                Files.copy(file.toPath(), outputStream);
                outputStream.flush();
            }
        }
    }

    private void setContType(String name, HttpServletResponse response) {
        name = name.toLowerCase();
        if (name.endsWith(".png")) {
            response.setContentType("image/png");
        } else if (name.endsWith(".jpg")) {
            response.setContentType("image/jpeg");
        } else if (name.endsWith(".html")) {
            response.setContentType("text/html");
        } else if (name.endsWith(".css")) {
            response.setContentType("text/css");
        } else if (name.endsWith(".js")) {
            response.setContentType("application/javascript");
        } else {
            throw new IllegalArgumentException("Can't find content type for '" + name + "'.");
        }
    }

}
