///\/\\////\///////\////\\\\\\/\\/\/\\////\///////\////\\\\\\/\\
//
//  Assignment       COMP4300 - Assignment 4
//  Professor:       David Churchill
//  Year / Term:     2021FALL
//  File Name:       Action.h
// 
//  Student Name:    Mahdi Rabie
//  Student User:    mr1826
//  Student Email:   mr1826@mun.ca
//  Student ID:      201025756
//  Group Member(s): [enter student name(s)]
//
///\/\\////\///////\////\\\\\\/\\/\/\\////\///////\////\\\\\\/\\
                              
#pragma once

#include "Common.h"
#include "Entity.h"
                              
class Action
{
    std::string m_name  = "NONE"; 
    std::string m_type  = "NONE"; 
    
public:

    Action();
    Action(const std::string & name, const std::string & type);

    const std::string & name() const;
    const std::string & type() const;
    std::string toString() const;
};


// Copyright (C) David Churchill - All Rights Reserved
// COMP4300 - 2021FALL - Assignment 4
// Written by David Churchill (dave.churchill@gmail.com)
// Unauthorized copying of these files are strictly prohibited
// Distributed only for course work at Memorial University
// If you see this file online please contact email above
