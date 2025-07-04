package com.example.bikeguard_backend.service;

import com.example.bikeguard_backend.model.User;
import com.example.bikeguard_backend.repository.UserRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

@Service
public class UserService {
    @Autowired
    private UserRepository userRepository;

    public User register(User user) {
        if (userRepository.findByEmail(user.getEmail()) != null) {
            throw new RuntimeException("Email already exists");
        }
        return userRepository.save(user);
    }

    public User login(String email, String password) {
        return userRepository.findByEmailAndPassword(email, password);
    }
}