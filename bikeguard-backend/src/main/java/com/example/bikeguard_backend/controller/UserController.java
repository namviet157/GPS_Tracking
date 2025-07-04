package com.example.bikeguard_backend.controller;

import com.example.bikeguard_backend.model.User;
import com.example.bikeguard_backend.service.UserService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

@RestController
@RequestMapping("/api")
@CrossOrigin(origins = "*")
public class UserController {
    @Autowired
    private UserService userService;

    @PostMapping("/register")
    public User register(@RequestBody User user) {
        return userService.register(user);
    }

    @PostMapping("/login")
    public User login(@RequestBody User user) {
        User found = userService.login(user.getEmail(), user.getPassword());
        if (found == null) throw new RuntimeException("Invalid credentials");
        return found;
    }
}
